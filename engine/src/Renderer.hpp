#ifndef CORE_RENDERER_HPP
#define CORE_RENDERER_HPP


#define CORE_GL_DEBUG



#ifdef CORE_GL_DEBUG



#endif

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

namespace core {


struct TextureFacet;


class Renderer : public updateable<Renderer, void, RuntimeContext>, public initializable<Renderer, void, void, void, void>, public pausable<Renderer> {
	friend class Core;
public:

	Renderer() : _backgroundColor{ 0.0f, 0.0f, 0.0f, 1.0f }, _debugShowTextureBounds{ false }, _thisDrawableId{ 0 } {
		_sdlWindow = nullptr;
		_sdlRenderer = nullptr;
	};

	bool createImpl();
	bool initializeImpl();
	bool resetImpl();
	bool destroyImpl();

	void pauseImpl();

	void resumeImpl();

	void updateImpl(float dt, RuntimeContext& context);

	void setDepthTestFunction(GLenum test);
	
	void render();		

	void applyDrawableChange(DrawableChange& dc);
	
	static int renderThread(void* data);

	void destroyDrawable(Drawable& d);

	int getMinZIndex(int layerId) const;

	void setBackgroundColor(Color& color);

	void handleDebugEvent(DebugEvent& debugEvent);

	SDL_Renderer* sdlRenderer();	

	void cleanup();

	~Renderer();
	
	void setGlobalColorModulation(ColorTransform& transform);
	
	ColorTransform getGlobalColorModulation();

	static int getMinZIndex_bind(LuaState& lua);

	static int hideWindow_bind(LuaState& lua);

	static int showWindow_bind(LuaState& lua);

	static int setBackgroundColor_bind(LuaState& lua);

	static int fadeWindow_bind(LuaState& lua);

	static int setGlobalColorModulation_bind(LuaState& lua);

	static int getGlobalColorModulation_bind(LuaState& lua);

	void hideWindow();

	void showWindow();

	bool isMultithreaded();

	void multithreadRender();

	//temp

	void start();
	void end();
private:

	SDL_Thread* _renderThread;

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

	void _draw(Drawable& d);

	void _drawTexture(Drawable& d);

	void _drawPoly(Drawable& d);

	void _drawPoly(std::vector<int>& points, Color& color, ColorTransform& colorTransform, bool filled);

	void _drawPoly(GLfloat* v, unsigned numPoints, Color& color, ColorTransform& colorTransform, bool filled);

	bool _renderMultithreaded;

	SDL_SpinLock _drawableChangePtrLock;
	SDL_SpinLock _renderThreadLock;

	void _checkGlDebugLog();

	bool _initGlObjects();
	bool _didInitGlObjects;
	void _addDrawableChange(DrawableChange& dc);

	bool _writingToFirstQueue;
	int _maxPendingQueueDepth;
	bool _doRenderThread;

	SPSCQueue<DrawableChange>* _drawableChangeQueue1;
	SPSCQueue<DrawableChange>* _drawableChangeQueue2;

	SPSCQueue<DrawableChange>** _drawableChanges;
	SPSCQueue<DrawableChange>** _drawablePending;

	//std::vector<DrawableChange>* _drawableChanges;
	//std::vector<DrawableChange> _drawableChanges1;
	//std::vector<DrawableChange> _drawableChanges2;		

	int _thisDrawableId;	

	std::vector<RenderLayer> _layers;

	Color _backgroundColor;
	bool _changeBackgroundColorFlag;

	SDL_Window* _sdlWindow;
	bool _windowShown;
	SDL_Renderer* _sdlRenderer;	
	
	SDL_GLContext _sdlInitContext;
	SDL_GLContext _sdlGlContext;

	SDL_Rect _screenRect;	

	bool _debugShowTextureBounds;

	GLenum _depthTestFunction;

	int _maxFramesPerSecond;

	SDL_SpinLock _drawablesLock;


	VertexBufferObject<GLfloat> _textureVbo;
	

	ColorTransform _colorTransform;
	ColorTransform _tempColorTransform;
	bool _changeColorTransform;

	std::vector<GLfloat> _drawColor;
	std::vector<GLfloat> _textureVertices;
	std::vector<GLfloat> _drawVertices;


	GLint _vertexPosVarLoc;

	SDL_Rect _windowRect;

	bool _debugOpenGl;

};



} //end namespace core

#endif // RENDERER_H_INCLUDED
