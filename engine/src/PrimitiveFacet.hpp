#ifndef CORE_PRIMITIVE_FACET_HPP
#define CORE_PRIMITIVE_FACET_HPP

#include "VisualFacet.hpp"
#include "Drawable.hpp"

namespace core {

	struct PrimitiveFacet : public VisualFacet {

		PrimitiveFacet() : color{ Color::CommonColor::WHITE } {
			setDeclaredTypeInfo(&typeid(*this));
		}
		

		Color color;
		Dimension dimensions;

		bool filled;


	};


} //end namespace core

#endif