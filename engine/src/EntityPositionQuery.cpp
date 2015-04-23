#include "EntityPositionQuery.hpp"
#include "EventProcessor.hpp"

namespace core {

	


	EntityPositionQuery::EntityPositionQuery() : found{ false } {
		
		lua_reg("entityId", &entity);
		lua_reg("found", &found);
		lua_reg("position", &position);
		lua_reg("dimensions", &dimensions);
		lua_reg("orientation", &orientation);
	}


	std::string EntityPositionQuery::getEventTypeNameImpl() {
		return "EntityPositionQuery";
	}



	bool EntityPositionQuery::createFromLua(LuaState& lua) {
		
		auto epq = EntityPositionQuery{};
		epq.fromLua(lua);
		single<EventProcessor>().process(epq);
		epq.toLua(lua);
		return true;

	}


} //end namespace core