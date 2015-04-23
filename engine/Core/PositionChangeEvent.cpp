#include "PositionChangeEvent.hpp"
#include "Core.hpp"
#include "EventProcessor.hpp"
#include "luaTable.hpp"

namespace core {


	PositionChangeEvent::PositionChangeEvent() : relative{ false } {
		
		lua_reg("position", &position);
		lua_reg("entityId", &entity);	
		lua_reg("relative", &relative);
	}



	std::string PositionChangeEvent::getEventTypeNameImpl() {
		return "PositionChangeEvent";
	}

	bool PositionChangeEvent::createFromLua(LuaState& lua) {

		auto pce = PositionChangeEvent{};		
		pce.fromLua(lua);

		single<EventProcessor>().process(pce);
		
		return false;

	}

}