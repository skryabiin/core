#ifndef CORE_POSITION_HPP
#define CORE_POSITION_HPP

#include "Facet.hpp"
#include "Geometry.hpp"
#include "Event.hpp"
#include "LuaUtil.hpp"
#include "Entity.hpp"

namespace core {

	struct PositionFacet : public Facet{
		PositionFacet() : Facet{}, p{ 0, 0, 0 }, orientation{ 1.0f, 0.0f }, dim{ 0, 0 } {
			_typeInfo = &typeid(this);
		};

		Pixel p;
		Vec2 orientation;
		Dimension dim;


	};

	


} //end namespace core

#endif