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

namespace core {
	bool Renderer::createImpl() {

		info("Creating Renderer:");
		info("Initializing SDL window...");
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			error("Error on SDL_Init: ", SDL_GetError());
			return false;
		}


		auto& lua = single<Core>().lua();

		_debugOpenGl = lua("Config")["graphics"]["debugOpenGl"];



		std::string title = lua("Config")["window"]["title"];


		int winw = lua("Config")["window"]["dimensions"][1];
		int winh = lua("Config")["window"]["dimensions"][2];
		

		bool borderless = lua("Config")["window"]["borderless"];
		bool fullscreen = lua("Config")["window"]["fullscreen"];
		bool centered = lua("Config")["window"]["centered"];
		bool resizable = lua("Config")["window"]["resizable"];
		bool grabMouse = lua("Config")["window"]["grabMouse"];

		int winx = (centered) ? SDL_WINDOWPOS_CENTERED : lua("Config")["window"]["position"][1];
		int winy = (centered) ? SDL_WINDOWPOS_CENTERED : lua("Config")["window"]["position"][2];

		
		_windowShown = false;
		info("Creating SDL window '", title, "' size ", winw, "x", winh);

		

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
		
		_sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winw, winh, flags);

		if (grabMouse) SDL_SetWindowGrab(_sdlWindow, SDL_TRUE);

		_screenRect = SDL_Rect{ 0, 0, winw, winh };
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

		lua.bindFunction("getMinZIndex_bind", Renderer::getMinZIndex_bind);
		lua.bindFunction("hideWindow_bind", Renderer::hideWindow_bind);
		lua.bindFunction("showWindow_bind", Renderer::showWindow_bind);
		lua.bindFunction("setBackgroundColor_bind", Renderer::setBackgroundColor_bind);
		lua.bindFunction("setGlobalColorModulation_bind", Renderer::setGlobalColorModulation_bind);

		_writingToFirstQueue = true;
		_drawableChangeQueue1 = new SPSCQueue<DrawableChange>{};
		_drawableChangeQueue2 = new SPSCQueue<DrawableChange>{};


		_doRenderThread = true;

		SDL_ShowCursor(0);

		_maxFramesPerSecond = lua("Config")["graphics"]["maxFramesPerSecond"];
		_renderMultithreaded = lua("Config")["graphics"]["renderMultithreaded"];
		_maxPendingQueueDepth = lua("Config")["graphics"]["maxWaitFreeQueueDepth"];

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

		auto renderShaderProgram = single<ShaderManager>().createShaderProgram("textureRender2d", "textureRender2d", "textureRender2d");
		checkStep = renderShaderProgram->link();
		if (!checkStep) return false;
		_textureVbo.create(2);
		_textureVbo.initialize();
		
		GLfloat fakeV[] = {
			0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f
		};

		_textureVbo.bind();
		_textureVbo.buffer(4, fakeV, GL_DYNAMIC_DRAW);		
		_textureVbo.unbind();

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
			if (drawable.disabled) continue;
			_draw(drawable);
			if (_debugOpenGl) {
				_checkGlDebugLog();
			}
		}
	}	
	SDL_AtomicUnlock(&_renderThreadLock);
	end();


}


void Renderer::_addDrawableChange(DrawableChange& dc) {

	auto currentPendingQueue = (_writingToFirstQueue) ? &_drawableChangeQueue1 : &_drawableChangeQueue2;
	(*currentPendingQueue)->enqueue(dc);
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


void Renderer::applyDrawableChange(DrawableChange& dc) {
	auto currentPendingQueue = (_writingToFirstQueue) ? &_drawableChangeQueue1 : &_drawableChangeQueue2;
	(*currentPendingQueue)->enqueue(dc);

}

void Renderer::_processDrawableChanges() {

	SDL_AtomicLock(&_drawableChangePtrLock);

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

		if (dc.operation == DrawableChange::Operation::CREATE_TEXTURE_DRAWABLE) {
			auto d = _createDrawable(dc.facetId, dc.layerId, dc.zIndex);
			_createTextureDrawable(d, dc);
			continue;
		}
		else if (dc.operation == DrawableChange::Operation::CREATE_PRIMITIVE_DRAWABLE) {
			auto d = _createDrawable(dc.facetId, dc.layerId, dc.zIndex);
			_createPrimitiveDrawable(d, dc);
			continue;
		}

		auto d = _getDrawable(dc.facetId, dc.layerId);
		switch (dc.operation) {
		case DrawableChange::Operation::PAUSE:		
			_pauseDrawable(d, dc);
			break;
		case DrawableChange::Operation::DESTROY_DRAWABLE:			
			_destroyDrawable(d, dc);
			break;
		case DrawableChange::Operation::CHANGE_COLOR:		
			_changeDrawableColor(d, dc);
			break;
		case DrawableChange::Operation::CHANGE_COLOR_TRANSFORM:			
			_changeDrawableColorTransform(d, dc);
			break;
		case DrawableChange::Operation::CHANGE_TEXTURE:			
			_changeDrawableTexture(d, dc);
			break;
		case DrawableChange::Operation::CHANGE_PRIMITIVE_POSITION:			
			_changePrimitivePosition(d, dc);
			break;
		case DrawableChange::Operation::CHANGE_TEXTURE_POSITION:		
			_changeTexturePosition(d, dc);
			break;
		}
	}
	//currentChangeQueue->clear();

	SDL_AtomicUnlock(&_drawableChangePtrLock);


}


void Renderer::handleDebugEvent(DebugEvent& debugEvent) {

	_debugShowTextureBounds = single<Console>().getDebugFlag("showTextureBounds");
}


Drawable* Renderer::_createDrawable(int facetId, short layerId, short zIndex) {

	auto d = Drawable{};
	d.facetId = facetId;
	d.layerId = layerId;
	d.zIndex = zIndex;

	for (auto& layer : _layers) {
		if (layer.layerId == layerId) {
			return layer.addDrawable(d);
		}
	}

	auto rl = RenderLayer{};
	rl.create(layerId);
	rl.initialize();	
	auto insIt = std::begin(_layers);
	for (auto it = std::begin(_layers); it != std::end(_layers); ++it) {
		if (rl.layerId > it->layerId) {
			insIt = it;
			break;
		}
	}
	insIt = _layers.insert(insIt, rl);
	return insIt->addDrawable(d);
}


void Renderer::_createTextureDrawable(Drawable* d, DrawableChange& dc) {

	d->layerId = dc.layerId;
	d->facetId = dc.facetId;
	d->camera = dc.camera;
	d->colorTransform = dc.colorTransform;
	d->zIndex = dc.zIndex;
	d->texture = dc.texture;
	d->targetRect = dc.targetRect;
	d->drawableType = Drawable::DrawableType::TEXTURE;
	d->sourceRect = dc.textureCoordinates;
	auto renderShaderProgram = single<ShaderManager>().getShaderProgram(dc.shaderProgramName);


	auto& vao = d->vao;
	vao.create();
	vao.initialize();
	vao.setProgram(renderShaderProgram);
	vao.bind();


	GLushort i[] = {
		0, 1, 2, 3
	};
	vao.indices().buffer(4, i);

	auto textureDim = d->texture->surfaceTrueDimensions();

	GLfloat uv[] = { dc.textureCoordinates.x, dc.textureCoordinates.y + dc.textureCoordinates.h,
		dc.textureCoordinates.x + dc.textureCoordinates.w, dc.textureCoordinates.y + dc.textureCoordinates.h,
		dc.textureCoordinates.x + dc.textureCoordinates.w, dc.textureCoordinates.y,
		dc.textureCoordinates.x, dc.textureCoordinates.y };


	uv[0] = uv[0] / (1.0 * textureDim.w);
	uv[1] = uv[1] / (1.0 * textureDim.h);
	uv[2] = uv[2] / (1.0 * textureDim.w);
	uv[3] = uv[3] / (1.0 * textureDim.h);
	uv[4] = uv[4] / (1.0 * textureDim.w);
	uv[5] = uv[5] / (1.0 * textureDim.h);
	uv[6] = uv[6] / (1.0 * textureDim.w);
	uv[7] = uv[7] / (1.0 * textureDim.h);
	d->uvbo.create(2);
	d->uvbo.initialize();
	d->uvbo.bind();
	d->uvbo.buffer(4, uv, GL_DYNAMIC_DRAW);
	vao.setVertexArrayAttribute("uvIn", d->uvbo);

	d->vbo.create(2);
	d->vbo.initialize();
	d->vbo.bind();
	auto vertexValues = std::vector<GLfloat>{};
	d->camera->getVertices(d->targetRect, vertexValues);
	d->vbo.buffer(vertexValues);	
	vao.setVertexArrayAttribute("vertexPos", d->vbo);
	vao.setUniformVariableMatrix("mvp", d->camera->getViewProjection());

	vao.unbind();
	d->uvbo.unbind();
	d->vbo.unbind();

}
void Renderer::_createPrimitiveDrawable(Drawable* d, DrawableChange& dc) {

	d->layerId = dc.layerId;
	d->facetId = dc.facetId;
	d->camera = dc.camera;
	d->colorTransform = dc.colorTransform;
	d->color = dc.color;
	d->zIndex = dc.zIndex;
	d->drawableType = Drawable::DrawableType::POLYGON;
	d->vertices = dc.vertices;
	auto shaderProgram = single<ShaderManager>().getShaderProgram(dc.shaderProgramName);

	auto& vao = d->vao;
	vao.create();
	vao.initialize();
	vao.setProgram(shaderProgram);
	vao.bind();

	auto indices = std::vector<GLushort>{};
	for (GLushort i = 0; i < 4; i++) {
		indices.push_back(i);
	}
	vao.indices().buffer(4, &indices[0]);
	vao.unbind();
}
void Renderer::_changeDrawableColor(Drawable* d, DrawableChange& dc) {

	d->color = dc.color;
	d->filled = dc.filled;
}
void Renderer::_changeDrawableColorTransform(Drawable* d, DrawableChange& dc) {

	d->colorTransform = dc.colorTransform;
}

void Renderer::_changeDrawableTexture(Drawable* d, DrawableChange& dc) {


	d->texture = dc.texture;
	auto& vao = d->vao;
	vao.bind();

	auto textureDim = d->texture->surfaceTrueDimensions();

	GLfloat uv[] = { dc.textureCoordinates.x, dc.textureCoordinates.y + dc.textureCoordinates.h,
		dc.textureCoordinates.x + dc.textureCoordinates.w, dc.textureCoordinates.y + dc.textureCoordinates.h,
		dc.textureCoordinates.x + dc.textureCoordinates.w, dc.textureCoordinates.y,
		dc.textureCoordinates.x, dc.textureCoordinates.y };


	uv[0] = uv[0] / (1.0 * textureDim.w);
	uv[1] = uv[1] / (1.0 * textureDim.h);
	uv[2] = uv[2] / (1.0 * textureDim.w);
	uv[3] = uv[3] / (1.0 * textureDim.h);
	uv[4] = uv[4] / (1.0 * textureDim.w);
	uv[5] = uv[5] / (1.0 * textureDim.h);
	uv[6] = uv[6] / (1.0 * textureDim.w);
	uv[7] = uv[7] / (1.0 * textureDim.h);
	d->uvbo.bind();
	d->uvbo.buffer(4, uv, GL_DYNAMIC_DRAW);
	vao.setVertexArrayAttribute("uvIn", d->uvbo);
	vao.unbind();
	d->uvbo.unbind();
}

void Renderer::_changeTexturePosition(Drawable* d, DrawableChange& dc) {
	d->targetRect = dc.targetRect;
	d->vao.bind();
	d->vbo.bind();
	auto vertexValues = std::vector<GLfloat>{};
	d->camera->getVertices(d->targetRect, vertexValues);
	d->vbo.buffer(vertexValues);
	d->vao.setVertexArrayAttribute("vertexPos", d->vbo);
	d->vao.unbind();
	d->vbo.unbind();
	if (d->zIndex != dc.zIndex) {
		_changeDrawableZIndex(d, dc);
	}
}

void Renderer::_changePrimitivePosition(Drawable* d, DrawableChange& dc) {
	d->vertices = dc.vertices;
	if (d->zIndex != dc.zIndex) {
		_changeDrawableZIndex(d, dc);
	}
}

void Renderer::_pauseDrawable(Drawable* d, DrawableChange& dc) {
	
	d->disabled = dc.paused;

}
void Renderer::_changeDrawableZIndex(Drawable* d, DrawableChange& dc) {
	
	auto newDrawable = *d;
	newDrawable.zIndex = dc.zIndex;
	for (auto dlit = std::begin(_layers); dlit != std::end(_layers); ++dlit) {
		if (dlit->layerId == dc.layerId) {
			dlit->destroyDrawable(d->facetId);
			dlit->addDrawable(newDrawable);

		}
	}
}


void Renderer::_destroyDrawable(Drawable* d, DrawableChange& dc) {
	d->vao.reset();
	d->vao.destroy();
	d->uvbo.reset();
	d->uvbo.destroy();
	for (auto dlit = std::begin(_layers); dlit != std::end(_layers); ++dlit) {
		if (dlit->layerId == dc.layerId) {
			dlit->destroyDrawable(d->facetId);
			return;
		}
	}
	warn("Attempting to destroy a non-existent drawable.");
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
	case Drawable::DrawableType::POLYGON:
		_drawPoly(d);
		break;
	case Drawable::DrawableType::TEXTURE:
		_drawTexture(d);
		break;
	}
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

void Renderer::_drawTexture(Drawable& d) {

	//if (!d.camera->positionRect(d.targetRect, _textureVertices)) return;	
	//will have to be more preicse for rotation

	d.vao.bind();

	//_textureVbo.bind();
	//d.camera->getVertices(d.targetRect, _textureVertices);
	//_textureVbo.buffer(4, &_textureVertices[0], GL_DYNAMIC_DRAW);

	//d.vao.setVertexArrayAttribute("vertexPos", _textureVbo);	

	auto finalColorTransform = d.colorTransform * _colorTransform;
	d.vao.setUniformVariableMatrix("colorTransform", finalColorTransform.transform);
	int samplerId = 0;
	d.vao.setUniformVariable<int>("textureSampler", samplerId);
	auto mvp = d.camera->getViewProjection();
	d.vao.setUniformVariableMatrix("mvp", mvp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, d.texture->getGlTextureId());
	d.vao.draw(GL_TRIANGLE_FAN);	

	d.vao.unbind();
	_textureVbo.unbind();
	

}


Drawable* Renderer::_getDrawable(int facetId, short layerId) {
	for (auto& layer : _layers) {
		if (layerId == layer.layerId || layerId == -1) {
			for (auto &drawable : layer.drawables) {
				if (drawable.facetId == facetId) {
					return &drawable;
				}
			}
		}
	}
	return nullptr;
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
			return layer.minZIndex();
		}
	}
	
	warn("Drawable layer not found for layerId ", layerId);
	return 0;
}

void Renderer::setGlobalColorModulation(ColorTransform& transform) {
	SDL_AtomicLock(&_drawableChangePtrLock);
	_tempColorTransform = transform;
	_changeColorTransform = true;
	SDL_AtomicUnlock(&_drawableChangePtrLock);
}

int Renderer::fadeWindow_bind(LuaState& lua) {

	float duration = lua.pullStack<float>();
	auto luaColorMod = LuaColorTransform(lua);
	LuaFunction lf = lua.pullStack<LuaFunction>();

	auto fader = RendererFadeTransition::createFromLua(lua);


	return 0;
}

ColorTransform Renderer::getGlobalColorModulation() {
	return (_changeColorTransform) ? _tempColorTransform : _colorTransform;
}

int Renderer::getGlobalColorModulation_bind(LuaState& lua) {

	auto colorTransform = LuaColorTransform{};
	colorTransform.setColorTransform(single<Renderer>().getGlobalColorModulation());

	lua.pushStack(colorTransform);
	return 1;

}

int Renderer::setGlobalColorModulation_bind(LuaState& lua) {

	auto luaColorMod = LuaColorTransform(lua);

	single<Renderer>().setGlobalColorModulation(luaColorMod.getColorTransform());

	return 0;
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
