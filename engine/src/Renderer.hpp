#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <algorithm>
#include <memory>
#include <GL/glew.h>
//include stdlib so glut can override exit()
#include <stdlib.h>
#include <glut.h>
#include <SDL.h>
#include "SDL_image.h"
#include "Templates.hpp"
#include "Config.hpp"
#include "Math.hpp"
#include "ResourceManager.hpp"
#include "Renderer2d.hpp"
#include "Animation.hpp"
#include "Camera2d.hpp"
#include "RuntimeContext.hpp"
#include "Texture.hpp"
#include "Drawable.hpp"
#include "Event.hpp"
#include "Console.hpp"
#include "EventFilter.hpp"
#include "DebugEvent.hpp"
#include "ParticleField.hpp"
#include "LinearParticleField.hpp"

namespace core {


struct TextureFacet;

class Renderer : public initializable<Renderer, void, void>, public EventListener {

public:

	Renderer() : _backgroundColor{ 0, 0, 0, 1 }, _debugShowTextureBounds{ false }, _thisDrawableId{ 0 } {
		_sdlWindow = nullptr;
		_sdlRenderer = nullptr;
	};

    InitStatus initializeImpl();

	InitStatus resetImpl();	

	void pauseDrawable(int id);

	void resumeDrawable(int id);

	void setDepthTestFunction(GLenum test);

	void render(Drawable& drawable, Camera2d& camera);
	
	void render();	

	void render(Drawable& d);

	int createDrawable(Drawable d);

	Drawable getDrawable(int id);

	void updateDrawable(Drawable d);

	void destroyDrawable(int id);

	int getMaxZIndex(int layerId) const;

	void handleDebugEvent(DebugEvent& debugEvent);

	SDL_Renderer* sdlRenderer();	

	void cleanup();

	~Renderer();
	
	LinearParticleField* TEMP_particleTest;

	static int getMaxZIndex_bind(LuaState& lua);

	//temp

	void start();
	void end();
private:

	/*
template <typename Drawable_type>
void drawImpl(Drawable_type* drawableAspect, RuntimeContext& context) {
	drawTexture(drawableAspect->getDrawable(), context);
    }

	*/
	Renderer2d* getRenderer2d(int layerId);

	void updateDrawable(Renderer2d* drawableLayer, Drawable d);

	int createDrawable(Renderer2d* drawableLayer, Drawable d);

	int _thisDrawableId;

	std::vector<std::unique_ptr<Renderer2d>> _renderers;

	std::vector<Drawable> _updatedDrawables;

	Color _backgroundColor;

	SDL_Window* _sdlWindow;

	SDL_Renderer* _sdlRenderer;	
	
	SDL_GLContext _sdlGlContext;

	SDL_Rect _screenRect;

	EventFilter<DebugEvent> _debugFilter;

	bool _debugShowTextureBounds;

	GLenum _depthTestFunction;
};



} //end namespace core

#endif // RENDERER_H_INCLUDED
