#ifndef CORE_VISUAL_FACET_HPP
#define CORE_VISUAL_FACET_HPP

#include "Facet.hpp"
#include "Drawable.hpp"

namespace core {


	struct VisualFacet : public Facet {

		VisualFacet() : offset{ 0, 0, 0 }, scale{ 1.0, 1.0 } {
			Facet();
		}

		Vec2 scale;

		Pixel offset;

		Drawable drawable;

	};


} //end namespace core
#endif