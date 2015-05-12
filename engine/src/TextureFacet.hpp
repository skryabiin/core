#ifndef CORE_VISUAL_FACETS_H
#define CORE_VISUAL_FACETS_H


#include "VisualFacet.hpp"
#include "Geometry.hpp"
#include "TextureManager.hpp"
#include "Drawable.hpp"

namespace core {


	struct TextureFacet : public VisualFacet {

		TextureFacet() {
			setDeclaredTypeInfo(&typeid(*this));
		}
		
		Texture* texture;
		SDL_Rect textureCoordinates;
	};




} //end namespace core


#endif