#ifndef CORE_RENDER_LAYER_HPP
#define CORE_RENDER_LAYER_HPP

#include "Templates.hpp"
#include "Color.hpp"
#include "Drawable.hpp"
#include "FrameBuffer.hpp"
#include "DrawableChange.hpp"

namespace core {

	struct CreateLayerRequest {
		short layerId;
		Camera* camera;
	};

	class RenderLayer : public renderable<RenderLayer>, public pausable<RenderLayer>, public initializable<RenderLayer, short, void, void, void> {
	public:
		bool createImpl(short layerId1);
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		void pauseImpl();
		void resumeImpl();

		void renderImpl();

		short minZIndex() const;		
		void destroyDrawable(int facetId);

		FrameBuffer* frame();

		short layerId() const;

		void setCamera(Camera* camera);

		Drawable* getDrawable(int facetId);

		Camera* camera();

		void setTextureAtlas(Texture* atlas);

		Drawable* createDrawable(int facetId, short layerId, short zIndex);

		void createTextureDrawable(Drawable* d, DrawableChange& dc);

		void createPrimitiveDrawable(Drawable* d, DrawableChange& dc);

		void changeDrawableColor(Drawable* d, DrawableChange& dc);

		void changeDrawableColorTransform(Drawable* d, DrawableChange& dc);

		void changeDrawableTexture(Drawable* d, DrawableChange& dc);

		void pauseDrawable(Drawable* d, DrawableChange& dc);

		void changeDrawableZIndex(Drawable* d, DrawableChange& dc);

		void destroyDrawable(Drawable* d, DrawableChange& dc);

		void changeTexturePosition(Drawable* d, DrawableChange& dc);

		void changePrimitivePosition(Drawable* d, DrawableChange& dc);

	private:


		ColorTransform colorTransform;
		short _layerId;

		unsigned int _indexCounter;
		Camera* _camera;
								

		Texture* _textureAtlas;

		FrameBuffer _frame;

		std::vector<Drawable> _drawables;

		std::vector<unsigned int> _indicesToDraw;
		VertexArrayObject _vao;
		IndexBufferObject _ibo;
		VertexBufferObject<GLint> _vbo;
		VertexBufferObject<GLfloat> _uvbo;

		std::vector<GLfloat> _textureCoordinates;		
		std::vector<GLint> _vertexValues;
	};


} //end namespace core
#endif