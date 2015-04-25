#ifndef CORE_VISUAL_FACETS_H
#define CORE_VISUAL_FACETS_H


#include "Facet.hpp"
#include "Geometry.hpp"
#include "ResourceManager.hpp"
#include "Drawable.hpp"

namespace core {


	struct TextureFacet : public Facet {

		TextureFacet() : scale{ 1.0f, 1.0f }, offset{ 0, 0, 0 } {
			_typeInfo = &typeid(*this);
		}

		Vec2 scale;

		int width;
		int height;

		Pixel offset;		
		


		Drawable drawable;

	};




} //end namespace core


#endif