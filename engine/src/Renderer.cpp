#include "Renderer.hpp"
#include "SDLUtil.hpp"
#include "TextureFacet.hpp"
#include "Console.hpp"
#include "Core.hpp"
#include "PhysicsSystem.hpp"
#include "Event.hpp"
#include "Drawable.hpp"
#include "ParticleSystem2d.hpp"

namespace core {
	bool Renderer::createImpl() {

		info("Creating Renderer:");
		info("Initializing SDL window...");
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			error("Error on SDL_Init: ", SDL_GetError());
			return false;
		}


		auto& lua = single<Core>().lua();
		std::string title = lua("Config")["title"];
		_maxFramesPerSecond = lua("PerformanceConfig")["maxFramesPerSecond"];
		_renderMultithreaded = lua("PerformanceConfig")["renderMultithreaded"];
		_maxPendingQueueDepth = lua("PerformanceConfig")["maxWaitFreeQueueDepth"];
		int winw = lua("Config")["window"][1];
		int winh = lua("Config")["window"][2];				
		_windowShown = false;
		info("Creating SDL window '", title, "' size ", winw, "x", winh);

		if (_renderMultithreaded) {
			info("Renderer multithreaded mode enabled.");
		}
		else {
			info("Renderer multithreaded mode disabled.");
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

		_sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winw, winh, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);


		_screenRect = SDL_Rect{ 0, 0, 1024, 768 };
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

		if (glDebugMessageCallbackAMD != NULL) {
			info("AMD debug output extension is available.");
		}

		lua.bindFunction("getMinZIndex_bind", Renderer::getMinZIndex_bind);
		lua.bindFunction("hideWindow_bind", Renderer::hideWindow_bind);
		lua.bindFunction("showWindow_bind", Renderer::showWindow_bind);
		lua.bindFunction("setBackgroundColor_bind", Renderer::setBackgroundColor_bind);

		_writingToFirstQueue = true;
		_drawableChangeQueue1 = new SPSCQueue<DrawableChange>{};
		_drawableChangeQueue2 = new SPSCQueue<DrawableChange>{};


		_doRenderThread = true;

		SDL_ShowCursor(0);


		if (_renderMultithreaded) { 		
			pause();
			_renderThread = SDL_CreateThread(Renderer::renderThread, "renderThread", NULL);

		}

		return true;

	}

	int Renderer::renderThread(void* data) {



		auto& renderer = unsynchronized<Renderer>();

		renderer.multithreadRender();
		return 0;
	}


	bool Renderer::_initGlObjects() {
		_didInitGlObjects = true;
		SDL_GL_MakeCurrent(_sdlWindow, _sdlGlContext);

		glClearColor(0.2f, 0.0f, 0.0f, 1.0f);

		_depthTestFunction = GL_LESS;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthFunc(GL_ALWAYS);

		_drawColor.reserve(4);
		_textureVertices.reserve(8);
		_drawVertices.reserve(8);

		auto checkStep = true;
		_drawShaderProgram.create();
		auto inits = _drawShaderProgram.initialize();

		_defaultDrawVertexShader = single<ShaderManager>().getVertexShader("shape2d");
		_currentDrawVertexShader = _defaultDrawVertexShader;
		_drawShaderProgram.setVertexShader(_defaultDrawVertexShader);

		_defaultDrawFragmentShader = single<ShaderManager>().getFragmentShader("shape2d");
		_currentDrawFragmentShader = _defaultDrawFragmentShader;
		_drawShaderProgram.setFragmentShader(_defaultDrawFragmentShader);
		checkStep = _drawShaderProgram.link();
		if (!checkStep) return false;

		_renderShaderProgram.create();
		inits = _renderShaderProgram.initialize();
		if (inits != InitStatus::INIT_TRUE) {
			return false;
		}
		_defaultRenderVertexShader = single<ShaderManager>().getVertexShader("textureRender2d");
		_renderShaderProgram.setVertexShader(_defaultRenderVertexShader);
		_defaultRenderFragmentShader = single<ShaderManager>().getFragmentShader("textureRender2d");
		_renderShaderProgram.setFragmentShader(_defaultRenderFragmentShader);
		checkStep = _renderShaderProgram.link();
		if (!checkStep) return false;
		_textureVao.create();
		_textureVao.setProgram(&_renderShaderProgram);
		_textureVbo.create(2);
		_textureUvbo.create(2);
		_textureIbo.create(1);


		_textureIbo.initialize();
		_textureUvbo.initialize();
		_textureVbo.initialize();
		_textureVao.initialize(false);





		_textureVao.bind();
		_textureVao.enableSetAttributes();
		GLfloat fakeV[] = {
			0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f
		};

		_textureVbo.bind();
		_textureVbo.buffer(4, fakeV, GL_DYNAMIC_DRAW);


		_textureVao.setVertexArrayAttribute("vertexPos", _textureVbo);


		GLushort i[] = {
			0, 1, 2, 3
		};
		_textureIbo.bind();
		_textureIbo.buffer(4, i);
		_textureVao.setIndices(_textureIbo);

		GLfloat fakeUV[] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f
		};

		_textureUvbo.bind();
		_textureUvbo.buffer(4, fakeUV, GL_DYNAMIC_DRAW);
		_textureVao.setVertexArrayAttribute("uvIn", _textureUvbo);

		int samplerId = 0;
		_textureVao.setUniformVariable<int>("textureSampler", samplerId);
		_textureVao.setUniformVariableMatrix("colorTransform", _colorMatrix);

		_textureVao.unbind();
		_textureIbo.unbind();
		_textureUvbo.unbind();
		_textureVbo.unbind();
		_textureVao.disableSetAttributes();
		return true;

	}

	bool Renderer::isMultithreaded() {
		return _renderMultithreaded;
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
		SDL_WaitThread(_renderThread,NULL);		

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
	
	SDL_AtomicLock(&_renderThreadLock);	

	if (!_renderThread && _renderMultithreaded) return;
	for (auto& layer : _layers) {
		if (layer.isPaused()) continue;
		for (auto& drawable : layer.drawables) {
			if (!drawable.texture->name().compare("blueBalls")) {
				int x = 0;
			}
			if (drawable.disabled) continue;
			_checkGlDebugLog();
			_draw(drawable);
			_checkGlDebugLog();
		}
	}	
	SDL_AtomicUnlock(&_renderThreadLock);
	end();


}


void Renderer::_addDrawableChange(DrawableChange& dc) {

	auto currentPendingQueue = (_writingToFirstQueue) ? &_drawableChangeQueue1 : &_drawableChangeQueue2;
	(*currentPendingQueue)->enqueue(dc);
}
void Renderer::updateImpl(RuntimeContext& context) {

	auto pendingChangeQueue = (_writingToFirstQueue) ? &_drawableChangeQueue1 : &_drawableChangeQueue2;

	int pendingChangeQueueDepth = (*pendingChangeQueue)->queueDepth();

	//don't need to switch if there are no changes in the write queue
	if (pendingChangeQueueDepth == 0) return;

	auto currentChangeQueue = (_writingToFirstQueue) ? &_drawableChangeQueue2 : &_drawableChangeQueue1;
	if ((*currentChangeQueue)->queueDepth() > 0) {

		//check write queue depth.  If this is lower than the threshhold, we will continue with the same writing queue
		//hoping the render thread catches up without having to wait
		
		if ( pendingChangeQueueDepth > _maxPendingQueueDepth) {
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

void Renderer::_processDrawableChanges() {

	SDL_AtomicLock(&_drawableChangePtrLock);

	if (_changeBackgroundColorFlag) {
		glClearColor(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b, _backgroundColor.a);
		_changeBackgroundColorFlag = false;
	}

	auto currentChangeQueue = (_writingToFirstQueue) ? &_drawableChangeQueue2 : &_drawableChangeQueue1;	
	
	DrawableChange dc;	
	while ((*currentChangeQueue)->dequeue(dc)) {

		if (dc.operation == DrawableChange::Operation::CREATE) {
			_processCreateDrawable(dc);
		}
		if (dc.operation == DrawableChange::Operation::UPDATE) {
			_processUpdateDrawable(dc);
		}
		else if (dc.operation == DrawableChange::Operation::DESTROY) {
			_processDestroyDrawable(dc);
		}
		else if (dc.operation == DrawableChange::Operation::PAUSE) {
			_processPauseDrawable(dc);
		}
		else if (dc.operation == DrawableChange::Operation::RESUME) {
			_processResumeDrawable(dc);
		}		
	}
	//currentChangeQueue->clear();

	SDL_AtomicUnlock(&_drawableChangePtrLock);


}


void Renderer::handleDebugEvent(DebugEvent& debugEvent) {

	_debugShowTextureBounds = single<Console>().getDebugFlag("showTextureBounds");
}


void Renderer::pauseDrawable(const Drawable& d) {

	auto dc = DrawableChange{};
	dc.drawable = d;
	dc.operation = DrawableChange::Operation::PAUSE;
	_addDrawableChange(dc);
}

void Renderer::resumeDrawable(Drawable& d) {
	auto dc = DrawableChange{};
	dc.drawable = d;
	dc.operation = DrawableChange::Operation::RESUME;
	_addDrawableChange(dc);
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

void Renderer::setDepthTestFunction(GLenum test) {
	_depthTestFunction = test;
}


void Renderer::start() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Renderer::end() {
	SDL_GL_SwapWindow(_sdlWindow);
}

void Renderer::_draw(Drawable& d) {
	switch (d.drawableType) {
	case Drawable::DrawableType::RECTANGLE:
		_drawPoly(d);
		break;
	case Drawable::DrawableType::TEXTURE:
		_drawTexture(d);
		break;
	}
}

void Renderer::_drawPoly(Drawable& d) {
	if (d.camera->positionPoints(d.vertices, _drawVertices)) {
		_drawPoly(&_drawVertices[0], d.vertices.size(), d.color, d.colorTransform, d.filled);
	}
}

void Renderer::_drawPoly(GLfloat* v, unsigned numPoints, Color& color, ColorTransform& colorTransform, bool filled) {

	_drawShaderProgram.bind();
	auto vbo = VertexBufferObject<GLfloat>{};

	vbo.create(2);
	vbo.initialize();
	vbo.bind();

	vbo.buffer(numPoints, v);

	_drawShaderProgram.setVertexArrayAttribute("vertexPos", vbo);

	vbo.unbind();

	std::vector<GLushort> indices;
	for (unsigned short i = 0; i < numPoints; ++i) {
		indices.push_back(GLushort(i));
	}

	//generate index data
	auto ibo = IndexBufferObject{};
	ibo.create(1);
	ibo.initialize();
	ibo.bind();

	ibo.buffer(indices);

	ibo.unbind();

	_drawShaderProgram.setUniformVariableArray("drawColor", (glm::vec4)color);
	_drawShaderProgram.setUniformVariableMatrix("colorMatrix", (glm::mat4)colorTransform);
	GLenum mode = (filled) ? GL_TRIANGLE_FAN : GL_LINE_LOOP;

	_drawShaderProgram.drawElements(vbo, ibo, mode);

	_drawShaderProgram.unbind();
	ibo.unbind();
	vbo.unbind();
	ibo.reset();
	ibo.destroy();
	vbo.reset();
	vbo.destroy();
}


void Renderer::_drawTexture(Drawable& d) {

	bool onScreen = d.camera->positionRect(d.targetRect, _textureVertices);
	if (!onScreen) return;
	//will have to be more preicse for rotation

	auto textureDim = d.texture->surfaceTrueDimensions();
	_textureVao.bind();

	GLfloat uv[] = { d.sourceRect.x, d.sourceRect.y + d.sourceRect.h,
		d.sourceRect.x + d.sourceRect.w, d.sourceRect.y + d.sourceRect.h,
		d.sourceRect.x + d.sourceRect.w, d.sourceRect.y,
		d.sourceRect.x, d.sourceRect.y };


	uv[0] = uv[0] / (1.0 * textureDim.w);
	uv[1] = uv[1] / (1.0 * textureDim.h);
	uv[2] = uv[2] / (1.0 * textureDim.w);
	uv[3] = uv[3] / (1.0 * textureDim.h);
	uv[4] = uv[4] / (1.0 * textureDim.w);
	uv[5] = uv[5] / (1.0 * textureDim.h);
	uv[6] = uv[6] / (1.0 * textureDim.w);
	uv[7] = uv[7] / (1.0 * textureDim.h);

	_textureUvbo.bind();
	_textureUvbo.buffer(4, uv, GL_DYNAMIC_DRAW);
	_textureVao.setVertexArrayAttribute("uvIn", _textureUvbo);
	_textureUvbo.unbind();


	_textureVbo.bind();
	_textureVbo.buffer(4, &_textureVertices[0], GL_DYNAMIC_DRAW);
	_textureVao.setVertexArrayAttribute("vertexPos", _textureVbo);

	auto colorMatrix = _colorMatrix * d.colorTransform;
	_textureVao.setUniformVariableMatrix("colorTransform", colorMatrix.transform);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, d.texture->getGlTextureId());


	_textureVao.draw(GL_TRIANGLE_FAN);
	_textureVao.unbind();
	_textureVbo.unbind();
	_textureUvbo.unbind();
	_textureVao.disableSetAttributes();


}


Drawable* Renderer::_getDrawable(int id, int layerId) {
	for (auto& layer : _layers) {
		if (layerId == layer.layerId || layerId == -1) {
			for (auto &drawable : layer.drawables) {
				if (drawable.id == id) {
					return &drawable;
				}
			}
		}
	}
	return nullptr;
}

void Renderer::_processPauseDrawable(DrawableChange& dc) {	
	int id = dc.drawable.id;
	auto drawable = _getDrawable(id, -1);
	drawable->disabled = true;
}
void Renderer::_processResumeDrawable(DrawableChange& dc) {
	int id = dc.drawable.id;
	auto drawable = _getDrawable(id, -1);
	drawable->disabled = false;
}


void Renderer::_processCreateDrawable(DrawableChange& dc) {

	for (auto& layer : _layers) {
		if (layer.layerId == dc.drawable.layerId) {
			auto insIt = std::end(layer.drawables);

			bool newMinZ = true;
			for (auto it = std::begin(layer.drawables); it != std::end(layer.drawables); ++it) {
				if (dc.drawable.zIndex > it->zIndex) {
					insIt = it;
					newMinZ = false;
					break;
				}
			}
			auto out = layer.drawables.insert(insIt, dc.drawable);

			if (newMinZ) {
				layer.minZIndex = dc.drawable.zIndex;
			}
			return;
		}
	}

	auto rl = RenderLayer{};
	rl.create(dc.drawable.layerId);	
	rl.initialize();
	rl.minZIndex = dc.drawable.zIndex;
	rl.drawables.push_back(dc.drawable);
	auto insIt = std::begin(_layers);
	for (auto it = std::begin(_layers); it != std::end(_layers); ++it) {
		if (rl.layerId > it->layerId) {
			insIt = it;
			break;
		}
	}
	_layers.insert(insIt, rl);
	
}

void Renderer::_processUpdateDrawable(DrawableChange& dc) {

	bool updatedZIndex = false;
	int newZIndex = 0;
	int oldZIndex = 0;

	auto& d = dc.drawable;
	for (auto& layer : _layers) {
		if (layer.layerId == d.layerId) {
			for (auto it = std::begin(layer.drawables); it != std::end(layer.drawables); ++it) {
				if (it->id == d.id) {
					d.disabled = it->disabled;
					if (d.zIndex != it->zIndex) {
						updatedZIndex = true;
						layer.drawables.erase(it);
					}
					else {
						*it = d;
					}
					break;
				}
			}
			if (updatedZIndex) {

				auto insIt = std::end(layer.drawables);

				bool newMinZ = true;

				for (auto it2 = std::begin(layer.drawables); it2 != std::end(layer.drawables); ++it2) {
					if (d.zIndex > it2->zIndex) {
						insIt = it2;
						newMinZ = false;
						break;						
					}

				}

				layer.drawables.insert(insIt, d);
				if (newMinZ) {
					layer.minZIndex = d.zIndex;
				}
				return;
			}
		}

	}

}
void Renderer::_processDestroyDrawable(DrawableChange& dc) {
	for (auto dlit = std::begin(_layers); dlit != std::end(_layers); ++dlit) {				
		for (auto it = std::begin(dlit->drawables); it != std::end(dlit->drawables); ++it) {
			if (it->id == dc.drawable.id) {
				dlit->drawables.erase(it);				
				return;
			}
		}
	}
	warn("Attempting to destroy a non-existent drawable.");
}

int Renderer::createDrawable(Drawable& d) {

	d.id = ++_thisDrawableId;
	DrawableChange dc = DrawableChange{};
	dc.operation = DrawableChange::Operation::CREATE;
	dc.drawable = d;
	_addDrawableChange(dc);
	return d.id;
}



void Renderer::setBackgroundColor(Color& color) {

	SDL_AtomicLock(&_drawableChangePtrLock);
	_backgroundColor = color;
	_changeBackgroundColorFlag = true;
	SDL_AtomicUnlock(&_drawableChangePtrLock);

}


int Renderer::setBackgroundColor_bind(LuaState& lua) {

	auto color = LuaColor{};
	color.fromLua(lua);
	single<Renderer>().setBackgroundColor(color.getColor());
	return 0;

}

int Renderer::getMinZIndex(int layerId) const {

	for (auto& layer : _layers) {
		if (layer.layerId == layerId) {
			return layer.minZIndex;
		}
	}
	
	warn("Drawable layer not found for layerId ", layerId);
	return 0;
}

void Renderer::updateDrawable(Drawable& d) {

	auto dc = DrawableChange{};
	dc.drawable = d;
	dc.operation = DrawableChange::Operation::UPDATE;

	_addDrawableChange(dc);
}

void Renderer::destroyDrawable(Drawable& d) {
	auto dc = DrawableChange{};
	dc.drawable = d;
	dc.operation = DrawableChange::Operation::DESTROY;
	_addDrawableChange(dc);
}

int Renderer::hideWindow_bind(LuaState& lua) {
	single<Renderer>().hideWindow();
	return 0;
}

int Renderer::showWindow_bind(LuaState& lua) {
	single<Renderer>().showWindow();
	return 0;
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


SDL_Renderer* Renderer::sdlRenderer() {
	return _sdlRenderer;
}


 Renderer::~Renderer() {


}




} //end namespace core
