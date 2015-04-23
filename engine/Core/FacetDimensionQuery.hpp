#ifndef CORE_FACET_DIMENSION_QUERY_HPP
#define CORE_FACET_DIMENSION_QUERY_HPP

#include "Core.hpp"
#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {



	struct FacetDimensionQuery : public Event <FacetDimensionQuery> {


		FacetDimensionQuery();

		long facetId;

		Entity entity;

		bool found;

		LuaDimension dimensions;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

	};


} //end namespace core

#endif