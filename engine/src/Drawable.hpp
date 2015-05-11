#ifndef CORE_DRAWABLE_HPP
#define CORE_DRAWABLE_HPP

#include "Geometry.hpp"
#include "Entity.hpp"
#include "Facet.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "VertexArrayObject.hpp"

namespace core {

	class Texture;

	class RenderableSystem2d;

	struct Drawable {

		enum class DrawableType {
			CIRCLE,
			RECTANGLE,
			POLYGON,
			LINE,
			TEXTURE,
			PARTICLE
		};

		Drawable() : zIndex{ 0 }, angle{ 0 }, disabled{ false }, layerId{ 0 }, radius{ 0 }, filled{ false }, color{ 1.0f, 1.0f, 1.0f, 1.0f } {

			texture = nullptr;
		}
		

		Texture* texture;
		SDL_Rect sourceRect;
		Rect targetRect;

		short layerId;
		
		short zIndex;
		double angle;		

		bool disabled; 
		Camera* camera;

		int facetId;
		
		Facet* facet;

		ColorTransform colorTransform;

		Color color;

		std::vector<Point> vertices;
		int radius;
		bool filled;

		ShaderProgram* program;
		VertexArrayObject vao;
		VertexBufferObject<GLfloat> uvbo;
		VertexBufferObject<GLfloat> vbo;

		DrawableType drawableType;
	};


} //end namespace core

#endif