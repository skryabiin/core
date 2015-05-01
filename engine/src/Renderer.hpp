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

namespace core {


struct TextureFacet;


class Renderer : public updateable<Renderer, void, RuntimeContext>, public initializable<Renderer, void, void, void, void>, public pausable<Renderer> {

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

	void updateImpl(RuntimeContext& context);


	void pauseDrawable(const Drawable& d);

	void resumeDrawable(Drawable& d);

	void setDepthTestFunction(GLenum test);
	
	void render();		



	int createDrawable(Drawable& d);
	
	static int renderThread(void* data);

	void updateDrawable(Drawable& d);

	void destroyDrawable(Drawable& d);

	int getMinZIndex(int layerId) const;

	void handleDebugEvent(DebugEvent& debugEvent);

	SDL_Renderer* sdlRenderer();	

	void cleanup();

	~Renderer();
	
	LinearParticleField* TEMP_particleTest;

	static int getMinZIndex_bind(LuaState& lua);

	static int hideWindow_bind(LuaState& lua);

	static int showWindow_bind(LuaState& lua);

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
	void _processPauseDrawable(DrawableChange& dc);
	void _processResumeDrawable(DrawableChange& dc);
	void _processCreateDrawable(DrawableChange& dc);
	void _processUpdateDrawable(DrawableChange& dc);
	void _processDestroyDrawable(DrawableChange& dc);

	Drawable* _getDrawable(int id, int layerId);
	void _draw(Drawable& d);

	void _drawTexture(Drawable& d);

	void _drawPoly(Drawable& d);

	void _drawPoly(std::vector<int>& points, Color& color, ColorTransform& colorTransform, bool filled);

	void _drawPoly(GLfloat* v, unsigned numPoints, Color& color, ColorTransform& colorTransform, bool filled);

	bool _renderMultithreaded;
	SDL_SpinLock _drawableChangePtrLock;
	SDL_SpinLock _renderThreadLock;
	SDL_atomic_t _writingFirstQueueFlag;

	void _checkGlDebugLog();

	bool _initGlObjects();
	bool _didInitGlObjects;
	void _addDrawableChange(DrawableChange& dc);

	bool _writingToFirstQueue;
	
	bool _doRenderThread;
	
	std::vector<DrawableChange>* _drawableChanges;
	std::vector<DrawableChange> _drawableChanges1;
	std::vector<DrawableChange> _drawableChanges2;

	/*
template <typename Drawable_type>
void drawImpl(Drawable_type* drawableAspect, RuntimeContext& context) {
	drawTexture(drawableAspect->getDrawable(), context);
    }

	*/
	
	

	int _thisDrawableId;	

	std::vector<RenderLayer> _layers;

	Color _backgroundColor;

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

	GLuint _vertexBuffer;
	GLuint _indexBuffer;

	VertexArrayObject _textureVao;
	VertexBufferObject<GLfloat> _textureVbo;
	IndexBufferObject _textureIbo;
	VertexBufferObject<GLfloat> _textureUvbo;

	ColorTransform _colorMatrix;

	std::vector<GLfloat> _drawColor;
	std::vector<GLfloat> _textureVertices;
	std::vector<GLfloat> _drawVertices;

	VertexShader *_defaultRenderVertexShader;
	FragmentShader *_defaultRenderFragmentShader;

	VertexShader* _defaultDrawVertexShader;
	FragmentShader* _defaultDrawFragmentShader;

	VertexShader* _currentDrawVertexShader;
	FragmentShader* _currentDrawFragmentShader;

	ShaderProgram _renderShaderProgram;
	ShaderProgram _drawShaderProgram;

	GLint _vertexPosVarLoc;

	SDL_Rect _windowRect;

};



} //end namespace core

#endif // RENDERER_H_INCLUDED
