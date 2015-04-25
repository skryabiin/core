#include "Renderer2d.hpp"
#include "VertexBufferObject.hpp"

namespace core {



	InitStatus Renderer2d::initializeImpl(Renderer2dInitSettings settings) {

		_sdlWindow = settings.sdlWindow;		
		_windowRect = settings.windowRect;

		auto checkStep = true;
		auto inits = _drawShaderProgram.initialize();

		_defaultDrawVertexShader = single<ShaderManager>().getVertexShader("shape2d");
		_currentDrawVertexShader = _defaultDrawVertexShader;
		_drawShaderProgram.setVertexShader(_defaultDrawVertexShader);

		_defaultDrawFragmentShader = single<ShaderManager>().getFragmentShader("shape2d");
		_currentDrawFragmentShader = _defaultDrawFragmentShader;
		_drawShaderProgram.setFragmentShader(_defaultDrawFragmentShader);
		checkStep = _drawShaderProgram.link();
		if (!checkStep) return InitStatus::INIT_FAILED;
		

		inits = _renderShaderProgram.initialize();
		if (inits != InitStatus::INIT_TRUE) {
			return InitStatus::INIT_FAILED;
		}
		_defaultRenderVertexShader = single<ShaderManager>().getVertexShader("textureRender2d");
		_renderShaderProgram.setVertexShader(_defaultRenderVertexShader);
		_defaultRenderFragmentShader = single<ShaderManager>().getFragmentShader("textureRender2d");
		_renderShaderProgram.setFragmentShader(_defaultRenderFragmentShader);
		checkStep = _renderShaderProgram.link();
		if (!checkStep) return InitStatus::INIT_FAILED;		

		_textureVao.initialize(false);
		_textureVao.setProgram(&_renderShaderProgram);
		_textureVao.bind();




		GLfloat fakeV[] = {
			0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f
		};

		
		_textureVbo.initialize(2);
		_textureVbo.bind();
		_textureVbo.buffer(4, fakeV, GL_DYNAMIC_DRAW);
		_textureVao.enableSetAttributes();
		_textureVao.setVertexArrayAttribute("vertexPos", _textureVbo);

		GLfloat fakeUV[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};




		_textureUvbo.initialize(2);
		_textureUvbo.bind();
		_textureUvbo.buffer(4, fakeUV, GL_DYNAMIC_DRAW);


		int samplerId = 0;
		_textureVao.setUniformVariable<int>("textureSampler", samplerId);
		_textureVao.setVertexArrayAttribute("uvIn", _textureUvbo);

		_colorMatrix = glm::mat4(0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		_textureVao.setUniformVariableMatrix("colorTransform", _colorMatrix);


		GLushort i[] = {
			0, 1, 2, 3
		};

		_textureIbo.initialize(1);
		_textureIbo.bind();
		_textureIbo.buffer(4, i);	
		_textureVao.setIndices(_textureIbo);
		_textureVao.unbind();
		_textureIbo.unbind();
		_textureUvbo.unbind();		
		_textureVao.disableSetAttributes();
		maxZIndex = 0;
		layerId = settings.layerId;
		return InitStatus::INIT_TRUE;

	}

	InitStatus Renderer2d::resetImpl() {
		
		auto textureVaoResetStatus = _textureVao.reset();
		
		auto drawProgramResetStatus = _drawShaderProgram.reset();

		drawables.clear();
		drawables.shrink_to_fit();

		return (textureVaoResetStatus == InitStatus::RESET_FAILED || drawProgramResetStatus == InitStatus::RESET_FAILED) ? InitStatus::RESET_FAILED : InitStatus::INIT_FALSE;		
	}



	void Renderer2d::setWindow(SDL_Window* window, SDL_Rect& windowRect) {
		_sdlWindow = window;
		_windowRect = windowRect;
	}


	void Renderer2d::drawRect(SDL_Color& color, SDL_Rect& rect) {

		drawRect(color, rect, false);

	}

	void Renderer2d::drawRect(SDL_Color& color, SDL_Rect& rect, bool filled) {

		GLfloat v[] = { getScreenCoordX(rect.x), getScreenCoordY(rect.y),
			getScreenCoordX(rect.x + rect.w), getScreenCoordY(rect.y),
			getScreenCoordX(rect.x + rect.w), getScreenCoordY(rect.y + rect.h),
			getScreenCoordX(rect.x), getScreenCoordY(rect.y + rect.h) };
		drawPoly(color, v, 4, filled);

	}

	void Renderer2d::drawPoly(SDL_Color& color, GLfloat* v, unsigned numPoints, bool filled) {

		_drawShaderProgram.bind();
		auto vbo = VertexBufferObject<GLfloat>{};

		vbo.initialize(2);
		vbo.bind();

		vbo.buffer(numPoints, v);

		_drawShaderProgram.enableVertexArrayAttributes();

		_drawShaderProgram.setVertexArrayAttribute("vertexPos", vbo);

		vbo.unbind();

		std::vector<GLushort> indices;
		for (unsigned short i = 0; i < numPoints; ++i) {
			indices.push_back(GLushort(i));
		}

		//generate index data
		auto ibo = IndexBufferObject{};
		ibo.initialize(1);
		ibo.bind();

		ibo.buffer(indices);

		ibo.unbind();

		//create color data

		std::vector<GLfloat> glColor = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };


		_drawShaderProgram.setUniformVariableArray("drawColor", glColor);

		GLenum mode = (filled) ? GL_TRIANGLE_FAN : GL_LINE_LOOP;

		_drawShaderProgram.drawElements(vbo, ibo, mode);

		_drawShaderProgram.disableVertexArrayAttributes();
		_drawShaderProgram.unbind();
		ibo.unbind();
		vbo.unbind();
		ibo.reset();
		vbo.reset();

	}

	void Renderer2d::drawRectFilled(SDL_Color& color, SDL_Rect& rect) {
		drawRect(color, rect, true);
	}


	GLfloat Renderer2d::getScreenCoordX(int& x) const {
		auto floatx = GLfloat(x);
		return (floatx - _windowRect.w / 2.0f) / (_windowRect.w / 2.0f);
	}

	GLfloat Renderer2d::getScreenCoordY(int& y) const {
		auto floaty = GLfloat(y);
		return (floaty - _windowRect.h / 2.0f) / (_windowRect.h / 2.0f);
	}

	GLfloat Renderer2d::getScreenCoordX(int&& x) const {
		auto floatx = GLfloat(x);
		return (floatx - _windowRect.w / 2.0f) / (_windowRect.w / 2.0f);
	}

	GLfloat Renderer2d::getScreenCoordY(int&& y) const {
		auto floaty = GLfloat(y);
		return (floaty - _windowRect.h / 2.0f) / (_windowRect.h / 2.0f);
	}

	void Renderer2d::drawPoly(SDL_Color& color, std::vector<SDL_Point> points) {

		std::vector<GLfloat> v;

		for (auto& point : points) {
			v.push_back(GLfloat(point.x));
			v.push_back(GLfloat(point.y));
		}

		drawPoly(color, &v[0], points.size(), false);

	}

	void Renderer2d::drawPoyFilled(SDL_Color& color, std::vector<SDL_Point> points) {

		std::vector<GLfloat> v;

		for (auto& point : points) {
			v.push_back(GLfloat(point.x));
			v.push_back(GLfloat(point.y));
		}

		drawPoly(color, &v[0], points.size(), true);
	}


	void Renderer2d::drawTexture(Drawable& d) {
		
		auto alignedTarget = d.targetRect;
		d.camera->positionRect(alignedTarget);

		//will have to be more preicse for rotation
		if (!SDL_HasIntersection(&alignedTarget, &_windowRect)) return;

		GLfloat v[] = { getScreenCoordX(alignedTarget.x), getScreenCoordY(alignedTarget.y),
			getScreenCoordX(alignedTarget.x + alignedTarget.w), getScreenCoordY(alignedTarget.y),
			getScreenCoordX(alignedTarget.x + alignedTarget.w), getScreenCoordY(alignedTarget.y + alignedTarget.h),
			getScreenCoordX(alignedTarget.x), getScreenCoordY(alignedTarget.y + alignedTarget.h) };
		
		auto textureDim = d.texture->dimensions();
		

		/*GLfloat uv[] = { d.sourceRect.x / textureDim.w, d.sourceRect.y / textureDim.h,
			(d.sourceRect.x + d.sourceRect.w) / textureDim.w, d.sourceRect.y / textureDim.h,
			(d.sourceRect.x + d.sourceRect.w) / textureDim.w, (d.sourceRect.y + d.sourceRect.h) / textureDim.h,
			d.sourceRect.x / textureDim.w, (d.sourceRect.y + d.sourceRect.h) / textureDim.h };
		*/

		/*GLfloat uv[] = { d.sourceRect.x, d.sourceRect.y,
			d.sourceRect.x + d.sourceRect.w, d.sourceRect.y,
			d.sourceRect.x + d.sourceRect.w, d.sourceRect.y + d.sourceRect.h,
			d.sourceRect.x, d.sourceRect.y + d.sourceRect.h };
		*/


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
		_textureVbo.bind();
		_textureVbo.buffer(4, v, GL_DYNAMIC_DRAW);

		_textureUvbo.bind();
		_textureUvbo.buffer(4, uv, GL_DYNAMIC_DRAW);



		_textureVao.bind();
		_textureVao.enableSetAttributes();		
		_textureVao.setVertexArrayAttribute("vertexPos", _textureVbo);
		_textureVao.setVertexArrayAttribute("uvIn", _textureUvbo);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, d.texture->getGlTextureId());
		_textureVao.draw(GL_TRIANGLE_FAN);
		_textureVao.unbind();
		_textureVbo.unbind();
		_textureUvbo.unbind();
		_textureVao.disableSetAttributes();


	}

	void Renderer2d::setDrawVertexShader(VertexShader* vertexShader) {
		_currentDrawVertexShader = vertexShader;
	}

	void Renderer2d::setDrawFragmentShader(FragmentShader* fragmentShader) {
		_currentDrawFragmentShader = fragmentShader;

	}

	void Renderer2d::resetDrawShaders() {
		_currentDrawVertexShader = _defaultDrawVertexShader;
		_currentDrawFragmentShader = _defaultDrawFragmentShader;
	}
}