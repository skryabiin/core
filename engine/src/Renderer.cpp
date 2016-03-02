#include "Renderer.hpp"
#include "SDLUtil.hpp"
#include "TextureFacet.hpp"
#include "Console.hpp"
#include "Core.hpp"
#include "PhysicsSystem.hpp"
#include "Event.hpp"
#include "Drawable.hpp"
#include "ParticleSystem2d.hpp"
#include "RendererFadeTransition.hpp"
#include "World.hpp"

namespace core {
	bool Renderer::createImpl() {

		info("Creating Renderer:");
		info("Initializing SDL window...");
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			error("Error on SDL_Init: ", SDL_GetError());
			return false;
		}


		
		auto& config = single<Config>();
		_debugOpenGl = config.graphics.debugOpenGl;



		std::string title = config.window.title;


		
		_windowDimensions = config.window.dimensions.getDimension();

		bool borderless = config.window.borderless;
		bool fullscreen = config.window.fullscreen;
		bool centered = config.window.centered;
		bool resizable = config.window.resizable;
		bool grabMouse = config.window.grabMouse;

		auto windowPos = config.window.position.getPixel();

		int winx = (centered) ? SDL_WINDOWPOS_CENTERED : windowPos.x;
		int winy = (centered) ? SDL_WINDOWPOS_CENTERED : windowPos.y;

		
		_windowShown = false;
		info("Creating SDL window '", title, "' size ", _windowDimensions.w, "x", _windowDimensions.h);		

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
		if (_debugOpenGl) {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		}

		auto flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL;

		flags = (borderless) ? flags | SDL_WINDOW_BORDERLESS : flags;
		flags = (fullscreen) ? flags | SDL_WINDOW_FULLSCREEN : flags;
		flags = (resizable) ? flags | SDL_WINDOW_RESIZABLE : flags;
		
		_sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _windowDimensions.w, _windowDimensions.h, flags);

		if (grabMouse) SDL_SetWindowGrab(_sdlWindow, SDL_TRUE);

		
		if (_sdlWindow == NULL) {
			error("Error on SDL_CreateWindow: ", SDL_GetError());
			return false;
		}

		_sdlGlContext = SDL_GL_CreateContext(_sdlWindow);
		_sdlInitContext = SDL_GL_CreateContext(_sdlWindow);

		info("OpenGL version: ", glGetString(GL_VERSION));
		//Initialize GLEW
		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
			return false;
		}

		if (_debugOpenGl && glDebugMessageCallbackAMD != NULL) {
			info("AMD debug output extension is available.");
		}

		auto& lua = single<Core>().lua();
		lua.bindFunction("getMinZIndex_bind", Renderer::getMinZIndex_bind);
		lua.bindFunction("hideWindow_bind", Renderer::hideWindow_bind);
		lua.bindFunction("showWindow_bind", Renderer::showWindow_bind);
		lua.bindFunction("setBackgroundColor_bind", Renderer::setBackgroundColor_bind);
		lua.bindFunction("setGlobalColorModulation_bind", Renderer::setGlobalColorModulation_bind);

		_writingToFirstQueue = true;
		_drawableChangeQueue1 = new SPSCQueue<DrawableChange>{};
		_drawableChangeQueue2 = new SPSCQueue<DrawableChange>{};
		_createRenderLayerQueue = new SPSCQueue<CreateLayerRequest>{};

		_doRenderThread = true;

		SDL_ShowCursor(1);

		_maxFramesPerSecond = config.graphics.maxFramesPerSecond;
		_renderMultithreaded = config.graphics.renderMultithreaded;
		_maxPendingQueueDepth = config.graphics.maxWaitFreeQueueDepth;

		if (_renderMultithreaded) {
			info("Renderer multithreaded mode enabled.");
		}
		else {
			info("Renderer multithreaded mode disabled.");
		}

		if (_renderMultithreaded) { 		
			pause();
			_renderThread = SDL_CreateThread(Renderer::renderThread, "renderThread", NULL);

		}

		_finalPassVao.create();
		_finalPassVbo.create(2);
		_finalPassUvbo.create(2);

		return true;

	}


	bool Renderer::initializeImpl() {


		for (auto& layer : _layers) {
			layer.initialize();
		}


		SDL_AtomicLock(&_drawableChangePtrLock);

		_drawableChangeQueue1->clear();
		_drawableChangeQueue2->clear();

		SDL_AtomicUnlock(&_drawableChangePtrLock);
		resume();
		if (!_renderMultithreaded && !_didInitGlObjects) {
			_initGlObjects();
		}
		return true;
	}

	bool Renderer::resetImpl() {
		pause();
		SDL_AtomicLock(&_drawableChangePtrLock);
		for (auto& layer : _layers) {
			layer.reset();
			layer.destroy();
		}

		_layers.clear();

		_drawableChangeQueue1->clear();
		_drawableChangeQueue2->clear();


		SDL_AtomicUnlock(&_drawableChangePtrLock);

		//_checkGlDebugLog();
		return true;
	}

	bool Renderer::destroyImpl() {

		_doRenderThread = false;
		SDL_AtomicUnlock(&_renderThreadLock);
		SDL_WaitThread(_renderThread, NULL);

		delete _drawableChangeQueue1;
		delete _drawableChangeQueue2;
		SDL_DestroyWindow(_sdlWindow);
		return true;
	}



	void Renderer::pauseImpl() {
		SDL_AtomicLock(&_renderThreadLock);
	}

	void Renderer::resumeImpl() {
		SDL_AtomicUnlock(&_renderThreadLock);
	}

	void Renderer::updateImpl(float dt, RuntimeContext& context) {

		auto pendingChangeQueue = (_writingToFirstQueue) ? &_drawableChangeQueue1 : &_drawableChangeQueue2;

		int pendingChangeQueueDepth = (*pendingChangeQueue)->queueDepth();

		//don't need to switch if there are no changes in the write queue
		if (pendingChangeQueueDepth == 0) return;

		auto currentChangeQueue = (_writingToFirstQueue) ? &_drawableChangeQueue2 : &_drawableChangeQueue1;
		if ((*currentChangeQueue)->queueDepth() > 0) {

			//check write queue depth.  If this is lower than the threshhold, we will continue with the same writing queue
			//hoping the render thread catches up without having to wait

			if (pendingChangeQueueDepth > _maxPendingQueueDepth) {
				warn("Drawable read queue is not empty, and write queue has exceeded ", _maxPendingQueueDepth, " (currently ", pendingChangeQueueDepth, ")");
				while ((*currentChangeQueue)->queueDepth() > 0) {
					//wait for other queue to drain;
				}
				SDL_AtomicLock(&_drawableChangePtrLock);
				_writingToFirstQueue = !_writingToFirstQueue;
				SDL_AtomicUnlock(&_drawableChangePtrLock);
			}
		}
		else {
			SDL_AtomicLock(&_drawableChangePtrLock);
			_writingToFirstQueue = !_writingToFirstQueue;
			SDL_AtomicUnlock(&_drawableChangePtrLock);
		}
	}


	int Renderer::renderThread(void* data) {



		auto& renderer = unsynchronized<Renderer>();

		renderer.multithreadRender();
		return 0;
	}

	bool Renderer::isMultithreaded() {
		return _renderMultithreaded;
	}

	void Renderer::multithreadRender() {
		SDL_AtomicLock(&_renderThreadLock);
		SDL_AtomicUnlock(&_renderThreadLock);
		if (!_initGlObjects()) {
			fatal("Could not initialize gl objects in render thread.  Quitting.");
		}


		int diff = 0;
		Uint32 lastTick = SDL_GetTicks();
		Uint32 ticks = 0;
		bool tickMinimumMet = false;

		while (_doRenderThread) {

			_processDrawableChanges();
			ticks = SDL_GetTicks();
			diff = ticks - lastTick;
			if (diff < 1000 / _maxFramesPerSecond || _paused) {
				SDL_Delay(1);
				continue;
			}
			render();
		}
	}



	void Renderer::render() {
		
		if (!_renderMultithreaded) {
			_processDrawableChanges();
		}
		start();
		
		int samplerId = 0;
		SDL_AtomicLock(&_renderThreadLock);	
		glActiveTexture(GL_TEXTURE0);
		if (!_renderThread && _renderMultithreaded) return;
		for (auto& layer : _layers) {
			layer.render();
			layer.frame()->bindTexture();
			_finalPassVao.bind();
			_finalPassVao.setUniformVariable<int>("textureSampler", samplerId);
			_finalPassVao.setUniformVariableMatrix("globalTransform", _colorTransform.transform);
			//_finalPassVao.setUniformVariableMatrix("mvp", layer.camera->getProjectionLocked());
			_finalPassVao.draw(GL_TRIANGLE_FAN);
		}
		
		SDL_AtomicUnlock(&_renderThreadLock);
		if (_debugOpenGl) {
			_checkGlDebugLog();
		}
		end();


	}



	void Renderer::start() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void Renderer::end() {

		SDL_GL_SwapWindow(_sdlWindow);
	}



	void Renderer::applyDrawableChange(DrawableChange& dc) {
		auto currentPendingQueue = (_writingToFirstQueue) ? &_drawableChangeQueue1 : &_drawableChangeQueue2;
		(*currentPendingQueue)->enqueue(dc);

	}

	void Renderer::createRenderLayer(short layerId, Camera* camera) {

		auto clr = CreateLayerRequest{};
		clr.layerId = layerId;
		clr.camera = camera;
		_createRenderLayerQueue->enqueue(clr);
	}

	void Renderer::_addRenderLayer(short layerId, Camera* camera) {

		auto rl = RenderLayer{};
		rl.create(layerId);
		rl.setCamera(camera);		
		rl.setTextureAtlas(_textureAtlas);
		auto insIt = std::begin(_layers);
		for (auto it = std::begin(_layers); it != std::end(_layers); ++it) {
			if (rl.layerId() > it->layerId()) {
				insIt = it;
				break;
			}
		}
		SDL_AtomicLock(&_renderThreadLock);
		insIt = _layers.insert(insIt, std::move(rl));
		insIt->initialize();
		SDL_AtomicUnlock(&_renderThreadLock);
	}

	int Renderer::getMinZIndex(short layerId) const {

		for (auto& layer : _layers) {
			if (layer.layerId() == layerId) {
				return layer.minZIndex();
			}
		}

		warn("Drawable layer not found for layerId ", layerId);
		return 0;
	}

	void Renderer::setTextureAtlas(Texture* atlas) {
		_textureAtlas = atlas;
	}


	void Renderer::setBackgroundColor(Color& color) {

		SDL_AtomicLock(&_drawableChangePtrLock);
		_backgroundColor = color;
		_changeBackgroundColorFlag = true;
		SDL_AtomicUnlock(&_drawableChangePtrLock);

	}


	void Renderer::setGlobalColorModulation(ColorTransform& transform) {
		SDL_AtomicLock(&_drawableChangePtrLock);
		_tempColorTransform = transform;
		_changeColorTransform = true;
		SDL_AtomicUnlock(&_drawableChangePtrLock);
	}

	ColorTransform Renderer::getGlobalColorModulation() {
		return (_changeColorTransform) ? _tempColorTransform : _colorTransform;
	}

	void Renderer::setDepthTestFunction(GLenum test) {
		_depthTestFunction = test;
	}


	Dimension Renderer::windowDimensions() const {

		return _windowDimensions;
	}


	void Renderer::hideWindow() {
		SDL_HideWindow(_sdlWindow);
	}
	void Renderer::showWindow() {
		SDL_ShowWindow(_sdlWindow);
	}



	int Renderer::getMinZIndex_bind(LuaState& lua) {

		int layerId = lua.pullStack<int>(1);
		lua.pushStack(single<Renderer>().getMinZIndex(layerId));
		return 1;
	}



	int Renderer::hideWindow_bind(LuaState& lua) {
		single<Renderer>().hideWindow();
		return 0;
	}

	int Renderer::showWindow_bind(LuaState& lua) {
		single<Renderer>().showWindow();
		return 0;
	}

	int Renderer::setBackgroundColor_bind(LuaState& lua) {

		auto color = LuaColor{};
		color.fromLua(lua);
		single<Renderer>().setBackgroundColor(color.getColor());
		return 0;

	}



	int Renderer::fadeWindow_bind(LuaState& lua) {

		float duration = lua.pullStack<float>();
		auto luaColorMod = LuaColorTransform(lua);
		LuaFunction lf = lua.pullStack<LuaFunction>();

		auto fader = RendererFadeTransition::createFromLua(lua);


		return 0;
	}

	int Renderer::setGlobalColorModulation_bind(LuaState& lua) {

		auto luaColorMod = LuaColorTransform(lua);

		single<Renderer>().setGlobalColorModulation(luaColorMod.getColorTransform());

		return 0;
	}


	int Renderer::getGlobalColorModulation_bind(LuaState& lua) {

		auto colorTransform = LuaColorTransform{};
		colorTransform.setColorTransform(single<Renderer>().getGlobalColorModulation());

		lua.pushStack(colorTransform);
		return 1;

	}




	void Renderer::_drawPoly(Drawable& d) {
		//if (!d.camera->positionPoints(d.vertices, _drawVertices)) return;	

		d.vao.bind();

		//_textureVbo.bind();
		//_textureVbo.buffer(d.vertices.size(), &_drawVertices[0]);

		//d.vao.setVertexArrayAttribute("vertexPos", _textureVbo);
		d.vao.setUniformVariableMatrix("colorTransform", _colorTransform.transform);
		d.vao.setUniformVariableArray("drawColor", (glm::vec4)d.color);

		d.vao.draw((d.filled) ? GL_TRIANGLE_FAN : GL_LINE_LOOP);

		d.vao.unbind();
		_textureVbo.unbind();
	}

	

	void Renderer::_addDrawableChange(DrawableChange& dc) {

		auto currentPendingQueue = (_writingToFirstQueue) ? &_drawableChangeQueue1 : &_drawableChangeQueue2;
		(*currentPendingQueue)->enqueue(dc);
	}



	void Renderer::_processDrawableChanges() {

		SDL_AtomicLock(&_drawableChangePtrLock);

		CreateLayerRequest clr;
		while (_createRenderLayerQueue->dequeue(clr)) {
			_addRenderLayer(clr.layerId, clr.camera);
		}

		if (_changeColorTransform) {
			_colorTransform = _tempColorTransform;
			_changeColorTransform = false;		
			_changeBackgroundColorFlag = true;
		}

		if (_changeBackgroundColorFlag) {
			auto background = _colorTransform * _backgroundColor;
			glClearColor(background.r, background.g, background.b, background.a);
			_changeBackgroundColorFlag = false;
		}
		auto currentChangeQueue = (_writingToFirstQueue) ? &_drawableChangeQueue2 : &_drawableChangeQueue1;	
	
		DrawableChange dc;	
		while ((*currentChangeQueue)->dequeue(dc)) {
			for (auto& layer : _layers) {
				if (layer.layerId() == dc.layerId) {
					if (dc.operation == DrawableChange::Operation::CREATE_TEXTURE_DRAWABLE) {
						auto d = layer.createDrawable(dc.facetId, dc.layerId, dc.zIndex);
						layer.createTextureDrawable(d, dc);
						continue;
					}
					else if (dc.operation == DrawableChange::Operation::CREATE_PRIMITIVE_DRAWABLE) {
						auto d = layer.createDrawable(dc.facetId, dc.layerId, dc.zIndex);
						layer.createPrimitiveDrawable(d, dc);
						continue;
					}

					auto d = layer.getDrawable(dc.facetId);
					switch (dc.operation) {
					case DrawableChange::Operation::PAUSE:
						layer.pauseDrawable(d, dc);
						break;
					case DrawableChange::Operation::DESTROY_DRAWABLE:
						layer.destroyDrawable(d, dc);
						break;
					case DrawableChange::Operation::CHANGE_COLOR:
						layer.changeDrawableColor(d, dc);
						break;
					case DrawableChange::Operation::CHANGE_COLOR_TRANSFORM:
						layer.changeDrawableColorTransform(d, dc);
						break;
					case DrawableChange::Operation::CHANGE_TEXTURE:
						layer.changeDrawableTexture(d, dc);
						break;
					case DrawableChange::Operation::CHANGE_PRIMITIVE_POSITION:
						layer.changePrimitivePosition(d, dc);
						break;
					case DrawableChange::Operation::CHANGE_TEXTURE_POSITION:
						layer.changeTexturePosition(d, dc);
						break;
					}
					break;
				}
			}
		}
		//currentChangeQueue->clear();

		SDL_AtomicUnlock(&_drawableChangePtrLock);


	}







	bool Renderer::_initGlObjects() {
		_didInitGlObjects = true;
		SDL_GL_MakeCurrent(_sdlWindow, _sdlGlContext);

		auto& config = single<Config>();

		//set vsync options
		auto vsyncStatus = SDL_GL_SetSwapInterval(config.graphics.vsync ? -1 : 0);
		
		//if that failed, late swap is not supported, so regular vsync
		if (vsyncStatus == -1)
			SDL_GL_SetSwapInterval(1);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		_depthTestFunction = GL_LESS;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthFunc(GL_ALWAYS);

		_drawColor.reserve(4);
		_textureVertices.reserve(8);
		_drawVertices.reserve(8);

		auto checkStep = true;

		auto drawShaderProgram = single<ShaderManager>().createShaderProgram("shape2d", "shape2d", "shape2d");
		checkStep = drawShaderProgram->link();
		if (!checkStep) return false;

	
		_textureVbo.create(2);
		_textureVbo.initialize(GL_DYNAMIC_DRAW);

		GLfloat defaultV[] = {
			-1.0f, -1.0f,
			1.0f, -1.0f,
			1.0f, 1.0f,
			-1.0f, 1.0f
		};

		_textureVbo.bind();
		_textureVbo.buffer(4, defaultV);
		_textureVbo.unbind();


		auto finalPassShaderProgram = single<ShaderManager>().createShaderProgram("finalPass2d", "finalPass2d", "finalPass2d");
		checkStep = finalPassShaderProgram->link();
		if (!checkStep) return false;


		_finalPassVao.initialize();
		_finalPassVao.setProgram(finalPassShaderProgram);
		_finalPassVao.bind();

		_finalPassVbo.initialize(GL_DYNAMIC_DRAW);
		_finalPassVbo.bind();
		_finalPassVbo.buffer(4, defaultV);
		_finalPassVao.setVertexArrayAttribute("vertexPos", _finalPassVbo);

		GLuint i[] = {
			0, 1, 2, 3
		};
		_finalPassVao.indices().buffer(4, i);

		_finalPassUvbo.initialize(GL_DYNAMIC_DRAW);
		_finalPassUvbo.bind();
		GLfloat uv[] = { 0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f };
		_finalPassUvbo.buffer(4, uv);
		_finalPassVao.setVertexArrayAttribute("uvIn", _finalPassUvbo);
		_finalPassVao.unbind();


		_finalPassUvbo.unbind();
		_finalPassVbo.unbind();
		return true;

	}


	

	void Renderer::_checkGlDebugLog()
	{
		unsigned int count = 10; // max. num. of messages that will be read from the log
		int bufsize = 2048;

		unsigned int* types = new unsigned int[count];
		unsigned int* ids = new unsigned int[count];
		unsigned int* severities = new unsigned int[count];
		int* lengths = new int[count];

		char* messageLog = new char[bufsize];

		unsigned int retVal = glGetDebugMessageLogAMD(count, bufsize, types, severities, ids,
			lengths, messageLog);
		if (retVal > 0)
		{
			unsigned int pos = 0;
			for (unsigned int i = 0; i<retVal; i++)
			{
				switch (severities[i]) {
				case GL_DEBUG_SEVERITY_HIGH_AMD:
					SDL_AtomicUnlock(&_renderThreadLock);
					fatal("OpenGL fatal - (type: ", types[i], ") (id: ", ids[i], ") ", &messageLog[pos]);
					_renderThread = false;
					delete[] types;
					delete[] ids;
					delete[] severities;
					delete[] lengths;
					delete[] messageLog;
					return;
					break;
				case GL_DEBUG_SEVERITY_MEDIUM_AMD:
					error("OpenGL error - (type: ", types[i], ") (id: ", ids[i], ") ", &messageLog[pos]);
					break;
				case GL_DEBUG_SEVERITY_LOW_AMD:
					warn("OpenGL warning - (type: ", types[i], ") (id: ", ids[i], ") ", &messageLog[pos]);
					break;
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					info("OpenGL info - (type: ", types[i], ") (id: ", ids[i], ") ", &messageLog[pos]);
					break;
				default:
					debug("OpenGL unkown severity - (type: ", types[i], ") (id: ", ids[i], ") ", &messageLog[pos]);
					break;
				}

				pos += lengths[i];
			}
		}

		delete[] types;
		delete[] ids;
		delete[] severities;
		delete[] lengths;
		delete[] messageLog;
	}




	





} //end namespace core
