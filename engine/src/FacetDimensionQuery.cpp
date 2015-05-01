#include "FacetDimensionQuery.hpp"
#include "EventProcessor.hpp"

namespace core {



	FacetDimensionQuery::FacetDimensionQuery() {

		lua_reg("entityId", &entity);
		lua_reg("facetId", &facetId);
		lua_reg("found", &found);
		lua_reg("dimensions", &dimensions);
	}

	std::string FacetDimensionQuery::getEventTypeNameImpl() {
		return "FacetDimensionQuery";

	}


	bool FacetDimensionQuery::createFromLua(LuaState& lua) {

		auto fdq = FacetDimensionQuery{};

		fdq.fromLua(lua);

		single<EventProcessor>().process(fdq);

		fdq.toLua(lua);
		return true;


	}


} //end namespace core