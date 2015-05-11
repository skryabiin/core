#include "VolumeChangeEvent.hpp"

namespace core {


	VolumeChangeEvent::VolumeChangeEvent() {

		lua_reg("entityId", &entity);
		lua_reg("facetId", &facetId);
		lua_reg("volume", &volume);
	}

	std::string VolumeChangeEvent::getEventTypeNameImpl() {
		return "VolumeChangeEvent";
	}

	bool VolumeChangeEvent::createFromLua(LuaState& lua) {

		auto pse = VolumeChangeEvent{};
		pse.fromLua(lua);
		single<EventProcessor>().process(pse);
		return false;
	}

} //end namespace core 