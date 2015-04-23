#ifndef CORE_RENDERER_2D_HPP
#define CORE_RENDERER_2D_HPP
#include <GL/glew.h>
//include stdlib so glut can override exit()
#include <stdlib.h>
#include <glut.h>
#include <SDL.h>
#include "Drawable.hpp"
#include "VertexShader.hpp"
#include "FragmentShader.hpp"
#include "ShaderProgram.hpp"
#include "VertexArrayObject.hpp"
#include "Templates.hpp"
#include "ShaderManager.hpp"

namespace core {

	struct Segment2D {
		SDL_Point a;
		SDL_Point b;

	};

	struct Renderer2dInitSettings {

		SDL_Window* sdlWindow;
		SDL_Rect windowRect;
		int layerId;

	};

	class Renderer2d : public initializable<Renderer2d, Renderer2dInitSettings, void> {

	public:

		InitStatus initializeImpl(Renderer2dInitSettings settings);

		InitStatus resetImpl();

		void setWindow(SDL_Window* window, SDL_Rect& windowRect);		

		void drawRect(SDL_Color& color, SDL_Rect& rect);

		void drawRect(SDL_Color& color, SDL_Rect& rect, bool filled);

		void drawRectFilled(SDL_Color& color, SDL_Rect& rect);

		void drawPoly(SDL_Color& color, std::vector<SDL_Point> points);

		void drawPoyFilled(SDL_Color& Color, std::vector<SDL_Point>points);

		void drawPoly(SDL_Color& color, GLfloat* v, unsigned numPoints, bool filled);		

		void setDrawVertexShader(VertexShader* vertexShader);
		
		void setDrawFragmentShader(FragmentShader* fragmentShader);

		void resetDrawShaders();

		void drawTexture(Drawable& d);

		GLfloat getScreenCoordX(int& x) const;

		GLfloat getScreenCoordY(int& y) const;

		GLfloat getScreenCoordX(int&& x) const;

		GLfloat getScreenCoordY(int&& y) const;


		short layerId;
		std::vector<Drawable> drawables;
		short maxZIndex;

	private:

		GLuint _vertexBuffer;
		GLuint _indexBuffer;

		VertexArrayObject _textureVao;
		VertexBufferObject<GLfloat> _textureVbo;
		IndexBufferObject _textureIbo;
		VertexBufferObject<GLfloat> _textureUvbo;		

		glm::mat4 _colorMatrix;

		VertexShader *_defaultRenderVertexShader;
		FragmentShader *_defaultRenderFragmentShader;

		VertexShader* _defaultDrawVertexShader;
		FragmentShader* _defaultDrawFragmentShader;

		VertexShader* _currentDrawVertexShader;
		FragmentShader* _currentDrawFragmentShader;

		ShaderProgram _renderShaderProgram;
		ShaderProgram _drawShaderProgram;

		SDL_Window* _sdlWindow;		
		GLint _vertexPosVarLoc;

		SDL_Rect _windowRect;

		
		
	};



} //end namespace core

#endif