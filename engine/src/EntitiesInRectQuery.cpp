#include "EntitiesInRectQuery.hpp"
#include "Core.hpp"
#include "EventProcessor.hpp"

namespace core {




	EntitiesInRectQuery::EntitiesInRectQuery() {

		lua_reg("rect", &rect);
		lua_reg("entities", &entities);
	}

	bool EntitiesInRectQuery::createFromLua(LuaState& lua) {

		auto eirq = EntitiesInRectQuery{};
		eirq.fromLua(lua);
		
		single<EventProcessor>().process(eirq);

		eirq.toLua(lua);

		return true;

	}

}