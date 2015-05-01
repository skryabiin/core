#include "OffsetChangeEvent.hpp"

namespace core {


	OffsetChangeEvent::OffsetChangeEvent() {
		lua_reg("entityId", &entity);
		lua_reg("offset", &offset);
		lua_reg("facetId", &facetId);
	}

	Entity entity;
	LuaPixel offset;

	std::string OffsetChangeEvent::getEventTypeNameImpl() {
		return "OffsetChangeEvent";
	}

	bool OffsetChangeEvent::createFromLua(LuaState& lua) {

		auto oce = OffsetChangeEvent{};
		oce.fromLua(lua);
		single<EventProcessor>().process(oce);
		return false;
	}


} //end namespace core