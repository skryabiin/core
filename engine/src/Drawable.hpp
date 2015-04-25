#ifndef CORE_DRAWABLE_HPP
#define CORE_DRAWABLE_HPP

#include "Geometry.hpp"
#include "Entity.hpp"
#include "Facet.hpp"
#include "Texture.hpp"
#include "Camera2d.hpp"
#include "RenderableSystem2d.hpp"

namespace core {

	class Texture;

	

	struct Drawable {

		Drawable() : zIndex{ 0 }, angle{ 0 }, disabled{ false }, doModulateColor{ false }, systemRendered{ false }, layerId{ 0 }  {
			texture = nullptr;
		}

		Texture* texture;
		SDL_Rect sourceRect;
		SDL_Rect targetRect;
		Color textureModulateColor;

		int layerId;

		bool doModulateColor;

		int zIndex;
		double angle;		


		int entityId;

		bool disabled; 
		Camera2d* camera;

		int id;

		

		bool systemRendered;
		Facet* facet;
		RenderableSystem2d* system;


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