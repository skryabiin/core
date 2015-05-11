#include "ColorModulationEvent.hpp"
#include "EventProcessor.hpp"

namespace core {


	ColorModulationEvent::ColorModulationEvent() : facetId{ -1 } {
		lua_reg("entityId", &entity);
		lua_reg("matrix", &matrix);
		lua_reg("facetId", &facetId);
	}


	std::string ColorModulationEvent::getEventTypeNameImpl() {
		return "ColorModulationEvent";
	}

	bool ColorModulationEvent::createFromLua(LuaState& lua) {


		auto cme = ColorModulationEvent{};
		cme.fromLua(lua);
		cme.transform = cme.matrix.getColorTransform();
		single<EventProcessor>().process(cme);
		return false;

	}


} //end namespace core