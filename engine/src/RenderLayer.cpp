#include "RenderLayer.hpp"
#include "Renderer.hpp"

namespace core {

	bool RenderLayer::createImpl(short layerId1) {
		_layerId = layerId1;
		_frame.create(single<Renderer>().windowDimensions());
		_indexCounter = 0;
		return true;
	}

	bool RenderLayer::initializeImpl() { 
		
		if (_frame.initialize() != InitStatus::INIT_TRUE) {
			return false;
		}
		_vbo.create(2);
		_vbo.initialize(GL_DYNAMIC_DRAW);
		_uvbo.create(2);
		_uvbo.initialize(GL_DYNAMIC_DRAW);
		auto renderShaderProgram = single<ShaderManager>().getShaderProgram("textureRender2d");
		if (renderShaderProgram == nullptr) {
			renderShaderProgram = single<ShaderManager>().createShaderProgram("textureRender2d", "textureRender2d", "textureRender2d");
			if (!renderShaderProgram->link()) {
				error("Could not retrieve/link texture shader program.");
			}
		}
		
		_vao.create();
		_vao.setProgram(renderShaderProgram);
		_vao.initialize();
		_vertexValues.reserve(10000);
		_textureCoordinates.reserve(10000);
		_indicesToDraw.reserve(10000);
		for (unsigned i = 0; i < 10000; i++) {
			_vertexValues.push_back(0);
			_textureCoordinates.push_back(0.0f);
		}
		
		_vbo.bind();
		_vbo.buffer(_vertexValues);

		_uvbo.bind();
		_uvbo.buffer(_textureCoordinates);

		_vao.unbind();
		_vbo.unbind();
		_uvbo.unbind();

		return true;  
	
	}
	bool RenderLayer::resetImpl() {
		bool check = (_frame.reset() != InitStatus::CREATE_TRUE);		
		_drawables.clear();
		return check;
	}
	bool RenderLayer::destroyImpl() { 		
		bool check = (_frame.destroy() != InitStatus::CREATE_FALSE);
		return check;
	}

	void RenderLayer::pauseImpl() {}
	void RenderLayer::resumeImpl() {}

	void RenderLayer::renderImpl() {
		if (isPaused()) return;
		_indicesToDraw.clear();
		float colors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		if (!_vao.bind()) {
			int x = 0;
		}
		_frame.bind();
		_frame.bindTexture();
		glClearBufferfv(GL_COLOR, 0, colors);
		int samplerId = 0;
		_vao.setUniformVariable<int>("textureSampler", samplerId);
		glBindTexture(GL_TEXTURE_2D, _textureAtlas->getGlTextureId());
		_uvbo.bind();
		_vao.setVertexArrayAttribute("uvIn", _uvbo);
		_vbo.bind();
		_vao.setVertexArrayAttribute("vertexPos", _vbo);
		_vao.setUniformVariableMatrix("mvp", _camera->getViewProjectionLocked());


		for (auto& d : _drawables) {
			if (d.disabled) continue;
			if (d.camera->isInViewportRect(d.targetRect)) {
				_indicesToDraw.clear();
				_indicesToDraw.push_back(d.bufferIndex * 4);
				_indicesToDraw.push_back(d.bufferIndex * 4 + 1);
				_indicesToDraw.push_back(d.bufferIndex * 4 + 2);
				_indicesToDraw.push_back(d.bufferIndex * 4 + 3);
				_vao.indices().buffer(_indicesToDraw);
				_vao.setUniformVariableMatrix("colorTransform", d.colorTransform.transform);
				_vao.draw(GL_TRIANGLE_FAN);
			}
		}
		_vao.unbind();
		_frame.unbind();
	}


	Drawable* RenderLayer::getDrawable(int facetId) {
		for (auto &drawable : _drawables) {
			if (drawable.facetId == facetId) {
				return &drawable;
			}
		}
		return nullptr;
	}

	FrameBuffer* RenderLayer::frame() {
		return &_frame;

	}
	void RenderLayer::setTextureAtlas(Texture* atlas) {
		_textureAtlas = atlas;
	}

	short RenderLayer::layerId() const {
		return _layerId;
	}

	void RenderLayer::setCamera(Camera* camera) {
		_camera = camera;
	}

	Camera* RenderLayer::camera() {
		return _camera;
	}

	short RenderLayer::minZIndex() const {
		return _drawables.back().zIndex;
	}

	void RenderLayer::destroyDrawable(int facetId) {
		for (auto it = std::begin(_drawables); it != std::end(_drawables); ++it) {
			if (it->facetId == facetId) {
				_drawables.erase(it);
				break;
			}
		}
	}

	Drawable* RenderLayer::createDrawable(int facetId, short layerId, short zIndex) {

		auto d = Drawable{};
		d.facetId = facetId;
		d.layerId = layerId;
		d.zIndex = zIndex;
		auto insIt = std::end(_drawables);

		for (auto it = std::begin(_drawables); it != std::end(_drawables); ++it) {
			if (d.zIndex > it->zIndex) {
				insIt = it;
				break;
			}
		}
		auto out = _drawables.insert(insIt, d);		
		return &(*out);

	}


	void RenderLayer::createTextureDrawable(Drawable* d, DrawableChange& dc) {

		d->layerId = dc.layerId;
		d->facetId = dc.facetId;
		d->camera = dc.camera;
		d->colorTransform = dc.colorTransform;
		d->zIndex = dc.zIndex;
		d->texture = dc.texture;
		d->targetRect = dc.targetRect;
		d->drawableType = Drawable::DrawableType::TEXTURE;
		d->bufferIndex = _indexCounter++;
		auto textureDim = SDL_Rect();
		if (dc.texture->isTextureAtlasManaged()) {
			auto newCoords = dc.textureCoordinates;
			auto texAtlasOrigin = dc.texture->getTextureAtlasOrigin();
			newCoords.x += texAtlasOrigin.x;
			newCoords.y += texAtlasOrigin.y;
			d->sourceRect = newCoords;			
		}
		
		int x1, x2, y1, y2;
		x1 = d->targetRect.x;
		x2 = d->targetRect.x + d->targetRect.w;
		y1 = d->targetRect.y;
		y2 = d->targetRect.y + d->targetRect.h;
		_vertexValues[d->bufferIndex * 8] = x1;
		_vertexValues[d->bufferIndex * 8 + 1] = y1;
		_vertexValues[d->bufferIndex * 8 + 2] = x2;
		_vertexValues[d->bufferIndex * 8 + 3] = y1;
		_vertexValues[d->bufferIndex * 8 + 4] = x2;
		_vertexValues[d->bufferIndex * 8 + 5] = y2;
		_vertexValues[d->bufferIndex * 8 + 6] = x1;
		_vertexValues[d->bufferIndex * 8 + 7] = y2;
		_vao.bind();
		_vbo.bind();
		_vbo.buffer(4, d->bufferIndex * 8, _vertexValues);

		//d->camera->getVertices(d->targetRect, vertexValues);		
		auto tdim = _textureAtlas->dimensions();

		float u1, u2, v1, v2;
		u1 = d->sourceRect.x * 1.0f / tdim.w;
		u2 = (d->sourceRect.x + d->sourceRect.w) * 1.0f / tdim.w;
		v1 = d->sourceRect.y * 1.0f / tdim.h;
		v2 = (d->sourceRect.y + d->sourceRect.h) * 1.0f / tdim.h;
		_textureCoordinates[d->bufferIndex * 8] = u1;
		_textureCoordinates[d->bufferIndex * 8 + 1] = v2;
		_textureCoordinates[d->bufferIndex * 8 + 2] = u2;
		_textureCoordinates[d->bufferIndex * 8 + 3] = v2;
		_textureCoordinates[d->bufferIndex * 8 + 4] = u2;
		_textureCoordinates[d->bufferIndex * 8 + 5] = v1;
		_textureCoordinates[d->bufferIndex * 8 + 6] = u1;
		_textureCoordinates[d->bufferIndex * 8 + 7] = v1;
		_uvbo.bind();
		_uvbo.buffer(4, d->bufferIndex * 8, _textureCoordinates);		
		_vao.unbind();
		_uvbo.unbind();
		_vbo.unbind();
				
	}

	void RenderLayer::createPrimitiveDrawable(Drawable* d, DrawableChange& dc) {

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
	}
	void RenderLayer::changeDrawableColor(Drawable* d, DrawableChange& dc) {

		d->color = dc.color;
		d->filled = dc.filled;
	}
	void RenderLayer::changeDrawableColorTransform(Drawable* d, DrawableChange& dc) {

		d->colorTransform = dc.colorTransform;
	}

	void RenderLayer::changeDrawableTexture(Drawable* d, DrawableChange& dc) {


		d->texture = dc.texture;
		
		auto newCoords = dc.textureCoordinates;
		auto texAtlasOrigin = dc.texture->getTextureAtlasOrigin();
		newCoords.x += texAtlasOrigin.x;
		newCoords.y += texAtlasOrigin.y;
		d->sourceRect = newCoords;			

		auto tdim = _textureAtlas->dimensions();

		float u1, u2, v1, v2;

		u1 = d->sourceRect.x * 1.0f / tdim.w;
		u2 = (d->sourceRect.x + d->sourceRect.w) * 1.0f / tdim.w;
		v1 = d->sourceRect.y * 1.0f / tdim.h;
		v2 = (d->sourceRect.y + d->sourceRect.h) * 1.0f / tdim.h;
		_textureCoordinates[d->bufferIndex * 8]     = u1;
		_textureCoordinates[d->bufferIndex * 8 + 1] = v2;
		_textureCoordinates[d->bufferIndex * 8 + 2] = u2;
		_textureCoordinates[d->bufferIndex * 8 + 3] = v2;
		_textureCoordinates[d->bufferIndex * 8 + 4] = u2;
		_textureCoordinates[d->bufferIndex * 8 + 5] = v1;
		_textureCoordinates[d->bufferIndex * 8 + 6] = u1;
		_textureCoordinates[d->bufferIndex * 8 + 7] = v1;
		_vao.bind();
		_uvbo.bind();
		_uvbo.buffer(4, d->bufferIndex * 8, _textureCoordinates);
		_uvbo.unbind();
		_vao.unbind();
	}




	void RenderLayer::pauseDrawable(Drawable* d, DrawableChange& dc) {

		d->disabled = dc.paused;

	}
	void RenderLayer::changeDrawableZIndex(Drawable* d, DrawableChange& dc) {

		auto newDrawable = *d;
		newDrawable.zIndex = dc.zIndex;
		destroyDrawable(d->facetId);
		auto insIt = std::end(_drawables);

		for (auto it = std::begin(_drawables); it != std::end(_drawables); ++it) {
			if (newDrawable.zIndex > it->zIndex) {
				insIt = it;
				break;
			}
		}
		_drawables.insert(insIt, newDrawable);
	}


	void RenderLayer::destroyDrawable(Drawable* d, DrawableChange& dc) {
		d->vao.reset();
		d->vao.destroy();
		d->uvbo.reset();
		d->uvbo.destroy();
		destroyDrawable(d->facetId);
	}

	void RenderLayer::changeTexturePosition(Drawable* d, DrawableChange& dc) {



		d->targetRect = dc.targetRect;
		
		if (d->zIndex != dc.zIndex) {
			changeDrawableZIndex(d, dc);
		}

		if (d->facetId == 11) {
			int x = 0;
		}
		int x1, x2, y1, y2;
		x1 = d->targetRect.x;
		x2 = d->targetRect.x + d->targetRect.w;
		y1 = d->targetRect.y;
		y2 = d->targetRect.y + d->targetRect.h;
		_vertexValues[d->bufferIndex * 8] = x1;
		_vertexValues[d->bufferIndex * 8 + 1] = y1;
		_vertexValues[d->bufferIndex * 8 + 2] = x2;
		_vertexValues[d->bufferIndex * 8 + 3] = y1;
		_vertexValues[d->bufferIndex * 8 + 4] = x2;
		_vertexValues[d->bufferIndex * 8 + 5] = y2;
		_vertexValues[d->bufferIndex * 8 + 6] = x1;
		_vertexValues[d->bufferIndex * 8 + 7] = y2;
		_vao.bind();
		_vbo.bind();
		_vbo.buffer(4, d->bufferIndex * 8, _vertexValues);
		_vao.unbind();
		_vbo.unbind();
	}



	void RenderLayer::changePrimitivePosition(Drawable* d, DrawableChange& dc) {
		d->vertices = dc.vertices;
		if (d->zIndex != dc.zIndex) {
			changeDrawableZIndex(d, dc);
		}
	}

} //end namespace core