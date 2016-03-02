#ifndef CORE_DRAWABLE_CHANGE_HPP
#define CORE_DRAWABLE_CHANGE_HPP


#include "Texture.hpp"
#include "Drawable.hpp"
#include "Geometry.hpp"

namespace core {


	
	struct DrawableChange {		
		
		enum class Operation {

			CREATE_TEXTURE_DRAWABLE,
			CREATE_PRIMITIVE_DRAWABLE,
			PAUSE,
			CHANGE_TEXTURE,
			CHANGE_COLOR,
			CHANGE_COLOR_TRANSFORM,
			DESTROY_DRAWABLE,
			CHANGE_TEXTURE_POSITION,
			CHANGE_PRIMITIVE_POSITION
			
		};

		Operation operation;
		short layerId;
		int facetId;

		bool paused;

		short zIndex;
		ColorTransform colorTransform;
		std::string shaderProgramName;
		Camera* camera;

		Texture* texture;
		SDL_Rect textureCoordinates;
		SDL_Rect targetRect;
		
		Color color;
		std::vector<Point> vertices;
		bool filled;
	};


} //end namespace core

#endif