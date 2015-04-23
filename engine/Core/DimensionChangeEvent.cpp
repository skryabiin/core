#include "DimensionChangeEvent.hpp"
#include "EventProcessor.hpp"

namespace core {


	DimensionChangeEvent::DimensionChangeEvent() {

		lua_reg("entityId", &entity);
		lua_reg("dimensions", &dimensions);		

	}

	std::string DimensionChangeEvent::getEventTypeNameImpl() {
		return "DimensionChangeEvent";
	}

	bool DimensionChangeEvent::createFromLua(LuaState& lua) {
		auto pce = DimensionChangeEvent{};
		pce.fromLua(lua);

		single<EventProcessor>().process(pce);

		return false;
	}

}