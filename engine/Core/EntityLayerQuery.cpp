#include "Core.hpp"
#include "EntityLayerQuery.hpp"
#include "EventProcessor.hpp"

namespace core {

	EntityLayerQuery::EntityLayerQuery() : layerId{ -1 }, found{ false } {
		lua_reg("entityId", &entity);
		lua_reg("layerId", &layerId);
		lua_reg("found", &found);

	};

	std::string EntityLayerQuery::getEventTypeNameImpl() {
		return "EntityLayerQuery";
	}

	bool EntityLayerQuery::createFromLua(LuaState& lua) {

		auto elq = EntityLayerQuery{};
		elq.fromLua(lua);
		single<EventProcessor>().process(elq);
		elq.toLua(lua);
		return true;
	}


}