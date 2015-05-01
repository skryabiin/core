#ifndef CORE_DRAWABLE_HPP
#define CORE_DRAWABLE_HPP

#include "Geometry.hpp"
#include "Entity.hpp"
#include "Facet.hpp"
#include "Texture.hpp"
#include "Camera2d.hpp"

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
		SDL_Rect targetRect;

		int layerId;
		
		int zIndex;
		double angle;		

		bool disabled; 
		Camera2d* camera;

		int id;
		
		Facet* facet;

		ColorTransform colorTransform;

		Color color;

		std::vector<Pixel> vertices;
		int radius;
		bool filled;

		DrawableType drawableType;
	};

	struct DrawableChange {

		enum class Operation {

			CREATE,
			UPDATE,
			DESTROY,
			PAUSE,
			RESUME
		};

		Operation operation;		
		Drawable drawable;

	};	

} //end namespace core

#endif