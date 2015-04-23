#ifndef CORE_INTERFACE_FACET_HPP
#define CORE_INTERFACE_FACET_HPP

#include "Facet.hpp"

namespace core {


	struct InterfaceFacet : public Facet {

		InterfaceFacet() : draggable{ false } {
			setDeclaredTypeInfo(&typeid(*this));
		}

		bool draggable;
		bool hoverable;
		bool clickable;
		int layerId;
		LuaFunction onClick;
		LuaFunction onDrag;

	};


} //end namespace core

#endif