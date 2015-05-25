#ifndef CORE_RENDERER_HPP
#define CORE_RENDERER_HPP



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
#include "TextureManager.hpp"
#include "Animation.hpp"
#include "Camera2d.hpp"
#include "RuntimeContext.hpp"
#include "Texture.hpp"
#include "Drawable.hpp"
#include "DrawableChange.hpp"
#include "Event.hpp"
#include "Console.hpp"
#include "DebugEvent.hpp"
#include "ParticleField.hpp"
#include "LinearParticleField.hpp"
#include "RenderLayer.hpp"
#include "ShaderManager.hpp"
#include "VertexShader.hpp"
#include "FragmentShader.hpp"
#include "ShaderProgram.hpp"
#include "VertexArrayObject.hpp"
#include "SPSCQueue.hpp"
#include "FrameBuffer.hpp"


namespace core {


struct TextureFacet;


class Renderer : public updateable<Renderer, void, RuntimeContext>, public initializable<Renderer, void, void, void, void>, public pausable<Renderer> {
	friend class Core;
public:

	

	//initializable
	//-------------
	bool createImpl();
	//-initializes sdl video
	//-initializes open gl context attributes
	//-creates the sdl window
	//-generates gl contexts
	//-initializes glew
	//-checks for opengl debug flag in config
	//-creates drawable change queues
	//-creates rendering thread
	//-creates vao and program for render layer drawing
	//
	//
	bool initializeImpl();
	//-initializes layers
	//-clears drawable change queues
	//-unpauses render thread
	//
	//
	bool resetImpl();
	//-resets and destroys layers
	//-clears drawable change queues
	//
	//
	bool destroyImpl();
	//-signals render thread shutdown and waits
	//-deletes drawable change queues
	//-destroys sdl window


	//pausable
	//--------
	void pauseImpl();
	//locks the render thread
	//
	//
	void resumeImpl();
	//unlocks the render thread
	

	//updateable
	//----------
	void updateImpl(float dt, RuntimeContext& context);
	//switches the drawable change queue, and
	//waits for changes to be consumed if the queue depth
	//exceeds the threshhold defined at Config.graphics.maxWaitFreeQueueDepth



	static int renderThread(void* data);
	//the function sent to SDL_CreateThread, this runs the renderer when in multithreaded mode

	bool isMultithreaded();
	//returns true if this is in multithreaded rendering mode


	void multithreadRender();
	//the main loop for multithreaded rendering
	//processes drawable changes, regulates frame rate, and calls render()
	//this regulates the render thread when the renderer is first initialized


	void render();
	//this is the main render loop.
	//either called by multithreadRender (if in multithreaded mode) or Core (in singlethreaded mode)


	void start();
	//initializes the opengl draw calls for this frame


	void end();
	//ends the opengl draw calls for this frame and swaps buffers



	void applyDrawableChange(DrawableChange& dc);
	//add a change to a drawable to the queue, to be processed by the render thread at next cycle


	void createRenderLayer(int layerId, Camera* camera);
	//creates a new render layer with the given id and camera, and sorts it into the render layer list


	int getMinZIndex(int layerId) const;
	//returns the z element of the closest (z-wise) drawable in the specified layer


	void setTextureAtlas(Texture* atlas);
	//sets the gl id for the atlas texture


	void setBackgroundColor(Color& color);
	//sets the clear color for the renderer


	void setGlobalColorModulation(ColorTransform& transform);
	//sets the color modulation matrix that is used on each render layer (including the interface)


	ColorTransform getGlobalColorModulation();
	//returns the color modulation matrix that is used on each render layer (including the interface)


	void setDepthTestFunction(GLenum test);
	//sets the opengl depth test function. This isnt used much since the renderer does its own
	//depth sorting and thus can use easier transparency



	Dimension windowDimensions() const;
	//returns the dimensions of the sdl window


	void hideWindow();
	//hides the window using SDL_ShowWindow(false)


	void showWindow();
	//shows the window using SDL_ShowWindow(true)

	
	//lua function bindings
	//---------------------
	static int getMinZIndex_bind(LuaState& lua);
	//returns the z element of the closest drawable in the specified layer
	//not used?
	//
	static int hideWindow_bind(LuaState& lua);
	//hides the window using SDL_ShowWindow(false)
	//see Renderer.hideWindow()
	//
	//
	static int showWindow_bind(LuaState& lua);
	//shows the window using SDL_ShowWindow(true)
	//see Renderer.showWindow()
	//
	//
	static int setBackgroundColor_bind(LuaState& lua);
	//sets the clear color for the renderer. 
	//see Renderer.setBackgroundColor()
	//
	//
	static int fadeWindow_bind(LuaState& lua);
	//fades the global color modulation from the current matrix to a different one, over a duration of miliseconds
	//Also registers a callback with the transition manager, if specified
	//see Renderer.fadeWindow()
	//
	//
	static int setGlobalColorModulation_bind(LuaState& lua);
	//sets the color modulation matrix that is used on each render layer (including the interface)
	//see Renderer.setGlobalColorModulation()
	//
	//
	static int getGlobalColorModulation_bind(LuaState& lua);
	//returns the color modulation matrix that is used on each render layer (including the interface)
	//see Renderer.getGlobalColorModulation()
	



private:


	//multithreading
	SDL_Thread* _renderThread;

	bool _renderMultithreaded;

	SDL_SpinLock _drawableChangePtrLock;

	SDL_SpinLock _renderThreadLock;

	bool _doRenderThread;


	//rendering calls for various types
	void _draw(Drawable& d);

	void _drawTexture(Drawable& d);

	void _drawPoly(Drawable& d);

	void _drawPoly(std::vector<int>& points, Color& color, ColorTransform& colorTransform, bool filled);

	void _drawPoly(GLfloat* v, unsigned numPoints, Color& color, ColorTransform& colorTransform, bool filled);




	//layers
	std::vector<RenderLayer> _layers;


	//drawable changes
	void _addDrawableChange(DrawableChange& dc);

	void _processDrawableChanges();

	Drawable* _createDrawable(int facetId, short layerId, short zIndex);

	void _createTextureDrawable(Drawable* d, DrawableChange& dc);

	void _createPrimitiveDrawable(Drawable* d, DrawableChange& dc);

	void _changeDrawableColor(Drawable* d, DrawableChange& dc);

	void _changeDrawableColorTransform(Drawable* d, DrawableChange& dc);

	void _changeDrawableTexture(Drawable* d, DrawableChange& dc);

	void _pauseDrawable(Drawable* d, DrawableChange& dc);

	void _changeDrawableZIndex(Drawable* d, DrawableChange& dc);

	void _destroyDrawable(Drawable* d, DrawableChange& dc);

	void _changeTexturePosition(Drawable* d, DrawableChange& dc);

	void _changePrimitivePosition(Drawable* d, DrawableChange& dc);
	
	Drawable* _getDrawable(int facetId, short layerId);	

	SPSCQueue<DrawableChange>* _drawableChangeQueue1;

	SPSCQueue<DrawableChange>* _drawableChangeQueue2;

	SPSCQueue<DrawableChange>** _drawableChanges;

	SPSCQueue<DrawableChange>** _drawablePending;

	bool _writingToFirstQueue;

	int _maxPendingQueueDepth;

	

	

	
	//opengl things
	bool _initGlObjects();
	//-steals the context for the render thread
	//-sets the initial clear color, depth test, blend function
	//-creates default shader programs for primitive and texture drawing
	//-creates the default shader program for render layer drawing

	bool _didInitGlObjects;

	bool _debugOpenGl; //set from Config.graphics.debugOpenGl
	

	void _checkGlDebugLog(); 
	//if _debugOpenGl is true:
	//-checks opengl debug log
	//-writes messages to console
	//-quits if any fatal errors detected

	
	//objects for render layer drawing
	VertexArrayObject _finalPassVao;	
	VertexBufferObject<GLfloat> _finalPassVbo;
	VertexBufferObject<GLfloat> _finalPassUvbo;
	
	//the current texture atlas 
	Texture* _textureAtlas;

	//gl contexts
	SDL_GLContext _sdlInitContext;
	SDL_GLContext _sdlGlContext;

	
	GLenum _depthTestFunction; //current depth test function

	Color _backgroundColor; //current clear color
	bool _changeBackgroundColorFlag; //signals the render thread to set the clear color


	ColorTransform _colorTransform; //current globlal color transform
	ColorTransform _tempColorTransform; //used until the render thread sets the current transform
	bool _changeColorTransform; //signals the render thread to set the current transform


	SDL_Window* _sdlWindow; //the sdl window

	bool _windowShown; //whether the window is shown
	
	Dimension _windowDimensions; //the window dimensions	

	int _maxFramesPerSecond; //set in Config.graphics.maxFramesPerSecond 





	//pre-allocated objects for gl buffering work
	VertexBufferObject<GLfloat> _textureVbo;
	std::vector<GLfloat> _drawColor;
	std::vector<GLfloat> _textureVertices;
	std::vector<GLfloat> _drawVertices;



};



} //end namespace core

#endif
