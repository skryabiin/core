#include "FacetPauseEvent.hpp"
#include "Core.hpp"

namespace core {


	FacetPauseEvent::FacetPauseEvent() : paused{ false }, facetId{ -1L } {

		lua_reg("paused", &paused);
		lua_reg("entityId", &entity);
		lua_reg("facetId", &facetId);
	}

	std::string FacetPauseEvent::getEventTypeNameImpl() {
		return "FacetPauseEvent";
	}

	bool FacetPauseEvent::createFromLua(LuaState& lua) {
		auto fpe = FacetPauseEvent{};		

		fpe.fromLua(lua);

		single<EventProcessor>().process(fpe);

		return true;
	}
}