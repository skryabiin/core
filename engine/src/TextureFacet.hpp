#ifndef CORE_VISUAL_FACETS_H
#define CORE_VISUAL_FACETS_H


#include "VisualFacet.hpp"
#include "Geometry.hpp"
#include "ResourceManager.hpp"
#include "Drawable.hpp"

namespace core {


	struct TextureFacet : public VisualFacet {

		TextureFacet() {
			setDeclaredTypeInfo(&typeid(*this));
		}
		

	};




} //end namespace core


#endif