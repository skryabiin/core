#include "ColorChangeEvent.hpp"
#include "Core.hpp"
#include "EventProcessor.hpp"

namespace core {

	ColorChangeEvent::ColorChangeEvent() : facetId{ -1 } {

		lua_reg("entityId", &entity);
		lua_reg("color", &color);		
		
		lua_reg("facetId", &facetId);
	}


	std::string ColorChangeEvent::getEventTypeNameImpl() {
		return "ColorChangeEvent";
	}

	bool ColorChangeEvent::createFromLua(LuaState& lua) {

		auto cce = ColorChangeEvent{};

		cce.fromLua(lua);
		single<EventProcessor>().process(cce);

		return true;
	}


}