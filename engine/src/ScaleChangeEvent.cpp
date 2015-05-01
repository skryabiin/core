#include "ScaleChangeEvent.hpp"

namespace core {


	ScaleChangeEvent::ScaleChangeEvent() {
		lua_reg("entityId", &entity);
		lua_reg("scale", &scale);
		lua_reg("facetId", &facetId);
	}

	std::string ScaleChangeEvent::getEventTypeNameImpl() {
		return "ScaleChangeEvent";
	}

	bool ScaleChangeEvent::createFromLua(LuaState& lua) {

		auto sce = ScaleChangeEvent{};
		sce.fromLua(lua);
		single<EventProcessor>().process(sce);
		return false;
	}


} //end namespace core