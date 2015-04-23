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
	

	InitStatus Renderer::initializeImpl() {

		info("Initializing Renderer:");
		info("Initializing SDL window...");
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			error("Error on SDL_Init: ", SDL_GetError());
			return InitStatus::INIT_FAILED;
		}

		auto& lua = single<Core>().lua();
		std::string title = lua("Config")["title"];
		int winw = lua("Config")["window"][1];
		int winh = lua("Config")["window"][2];
		bool winshow = lua("Config")["window"]["show"];
		auto sdlWinShow = (winshow) ? SDL_WINDOW_SHOWN : SDL_WINDOW_HIDDEN;
		info("Creating SDL window '", title, "' size ", winw, "x", winh);
		if (!winshow)
		warn("Configuration 'Config.window.show' is set to false.  The window will not be displayed.");


		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		_sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winw, winh, sdlWinShow | SDL_WINDOW_OPENGL);

		_screenRect = SDL_Rect{ 0, 0, winw, winh };
        if(_sdlWindow == NULL) {
			error("Error on SDL_CreateWindow: ", SDL_GetError());
			return InitStatus::INIT_FAILED;
        }


		_sdlGlContext = SDL_GL_CreateContext(_sdlWindow);

		info("OpenGL version: ", glGetString(GL_VERSION));
		//Initialize GLEW
		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		_depthTestFunction = GL_LESS;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		SDL_ShowCursor(0);

		std::function<void(Renderer*, DebugEvent& debugEvent)> debugCallback = std::mem_fn(&Renderer::handleDebugEvent);
		_debugFilter.init(this, debugCallback);
		single<EventProcessor>().addFilter(&_debugFilter);

		lua.bindFunction("getMaxZIndex_bind", Renderer::getMaxZIndex_bind);

        return InitStatus::INIT_TRUE;

}

	InitStatus Renderer::resetImpl() {

		//do not allow reset yet
		return InitStatus::INIT_TRUE;
	}

void Renderer::handleDebugEvent(DebugEvent& debugEvent) {

	_debugShowTextureBounds = single<Console>().getDebugFlag("showTextureBounds");
}


void Renderer::pauseDrawable(int id) {
	if (id < 0) return;

	for (auto dlit = std::begin(_renderers); dlit != std::end(_renderers); ++dlit) {

		auto& drawableLayer = *dlit;
		for (auto it = std::begin(drawableLayer->drawables); it != std::end(drawableLayer->drawables); ++it) {
			if (it->id == id) {
				it->disabled = true;
				return;
			}
		}
	}
}

void Renderer::resumeDrawable(int id) {
	if (id < 0) return;

	for (auto dlit = std::begin(_renderers); dlit != std::end(_renderers); ++dlit) {

		auto& renderer = *dlit;
		for (auto it = std::begin(renderer->drawables); it != std::end(renderer->drawables); ++it) {
			if (it->id == id) {
				it->disabled = false;
				return;
			}
		}
	}
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



void Renderer::render() {

	start();

	glDepthFunc(GL_ALWAYS);

	for (auto dlit = std::begin(_renderers); dlit != std::end(_renderers); ++dlit) {
		auto& renderer = *dlit;

		for (auto it = std::begin(renderer->drawables); it != std::end(renderer->drawables); ++it) {
			if (!it->disabled) renderer->drawTexture(*it);
		}
	}

	glDepthFunc(_depthTestFunction);

	end();
}

void Renderer::render(Drawable& d) {

	if (!d.disabled) {

		if (d.systemRendered) {
			d.system->renderFacet(_sdlRenderer,d.facet);
			return;
		}

		if (d.doModulateColor) {
			d.texture->modulateTextureColor(d.textureModulateColor);
		}
		else {
			d.texture->restoreDefaultTextureColor();
		}

		auto alignedTarget = d.targetRect;
		d.camera->positionRect(alignedTarget);
		if (SDL_HasIntersection(&alignedTarget, &_screenRect)) {

			
		}
	}
}

int Renderer::createDrawable(Drawable d) {

	auto dl = getRenderer2d(d.layerId);
	return createDrawable(dl, d);
	
}

int Renderer::createDrawable(Renderer2d* renderer, Drawable d) {
	auto insIt = std::begin(renderer->drawables);

	d.id = ++_thisDrawableId;
	d.systemRendered = false;

	bool newMaxZ = true;
	for (auto it = std::rbegin(renderer->drawables); it != std::rend(renderer->drawables); ++it) {
		if (d.zIndex > it->zIndex) {
			insIt = it.base();
			newMaxZ = false;
			break;
		}
	}

	auto out = renderer->drawables.insert(insIt, d);
	if (newMaxZ) {
		renderer->maxZIndex = d.zIndex;
	}
	return d.id;


}


Renderer2d* Renderer::getRenderer2d(int layerId) {
	for (auto dlit = std::begin(_renderers); dlit != std::end(_renderers); ++dlit) {
		auto& dl = *dlit;
		if (dl->layerId == layerId) {
			return dl.get();
		}
	}

	auto newRenderer = new Renderer2d{};	
	auto rendererSettings = Renderer2dInitSettings{};
	rendererSettings.sdlWindow = _sdlWindow;
	rendererSettings.windowRect = _screenRect;
	rendererSettings.layerId = layerId;
	newRenderer->initialize(rendererSettings);

	auto insIt = std::begin(_renderers);
	for (auto it = std::rbegin(_renderers); it != std::rend(_renderers); ++it) {
		if (layerId > (*it)->layerId) {
			insIt = it.base();
			break;
		}
	}	

	auto& out = _renderers.insert(insIt, std::unique_ptr<Renderer2d>(newRenderer));
	return (*out).get();
}

Drawable Renderer::getDrawable(int id) {

	if (id < 0) return Drawable{};

	for (auto dlit = std::begin(_renderers); dlit != std::end(_renderers); ++dlit) {

		auto& renderer = *dlit;
		for (auto it = std::begin(renderer->drawables); it != std::end(renderer->drawables); ++it) {
			if (it->id == id) {
				return *it;
			}
		}
	}

	warn("Drawable not found for id ", id);
	return Drawable{};

}

int Renderer::getMaxZIndex(int layerId) const {

	for (auto dlit = std::begin(_renderers); dlit != std::end(_renderers); ++dlit) {
		if ((*dlit)->layerId == layerId) {
			return (*dlit)->maxZIndex;
		}
	}
	
	warn("Drawable layer not found for layerId ", layerId);
	return 0;
}

void Renderer::updateDrawable(Drawable d) {

	
	bool updatedZIndex = false;	
	int newZIndex = 0;
	int oldZIndex = 0;

	auto dl = getRenderer2d(d.layerId);
	updateDrawable(dl, d);

}

void Renderer::updateDrawable(Renderer2d* renderer, Drawable d) {

	bool updatedZIndex = false;
	int newZIndex = 0;
	int oldZIndex = 0;

	for (auto it = std::begin(renderer->drawables); it != std::end(renderer->drawables); ++it) {
		if (it->id == d.id) {

			if (d.zIndex != it->zIndex) {
				updatedZIndex = true;
				renderer->drawables.erase(it);
			}
			else {
				*it = d;
			}
			break;
		}
	}

	if (updatedZIndex) {

		auto insIt = std::begin(renderer->drawables);

		bool newMaxZ = true;

		for (auto it2 = std::rbegin(renderer->drawables); it2 != std::rend(renderer->drawables); ++it2) {
			if (d.zIndex > it2->zIndex) {
				insIt = it2.base();
				break;
			}
			newMaxZ = false;
		}

		renderer->drawables.insert(insIt, d);
		if (newMaxZ) {
			renderer->maxZIndex = d.zIndex;
		}
		

	}

}

void Renderer::destroyDrawable(int id) {
	for (auto dlit = std::begin(_renderers); dlit != std::end(_renderers); ++dlit) {

		for (auto it = std::begin((*dlit)->drawables); it != std::end((*dlit)->drawables); ++it) {
			if (it->id == id) {
				(*dlit)->drawables.erase(it);
				return;
			}
		}
	}
	warn("Attempting to destroy a non-existent drawable.");
}


int Renderer::getMaxZIndex_bind(LuaState& lua) {
	
	int layerId = lua.pullStack<int>(1);
	lua.pushStack(single<Renderer>().getMaxZIndex(layerId));
	return 1;
}


SDL_Renderer* Renderer::sdlRenderer() {
	return _sdlRenderer;
}

void Renderer::cleanup() {
	for (auto& renderer : _renderers) {
		renderer.reset();
	}	
}

 Renderer::~Renderer() {

	 cleanup();
	 single<EventProcessor>().removeFilter(&_debugFilter);
     //cleanup SDL things
     //destroy window
	 SDL_DestroyRenderer(_sdlRenderer);
     SDL_DestroyWindow( _sdlWindow );

}




} //end namespace core
