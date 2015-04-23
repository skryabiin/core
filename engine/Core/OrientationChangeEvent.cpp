#include "OrientationChangeEvent.hpp"
#include "EventProcessor.hpp"

namespace core {


	OrientationChangeEvent::OrientationChangeEvent() {

		lua_reg("entityId", &entity);
		lua_reg("orientation", &orientation);

	}

	std::string OrientationChangeEvent::getEventTypeNameImpl() {
		return "OrientationChangeEvent";
	}

	bool OrientationChangeEvent::createFromLua(LuaState& lua) {
		auto pce = OrientationChangeEvent{};
		pce.fromLua(lua);

		single<EventProcessor>().process(pce);

		return false;
	}

}