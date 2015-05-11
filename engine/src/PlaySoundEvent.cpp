#include "PlaySoundEvent.hpp"

namespace core {



	PlaySoundEvent::PlaySoundEvent() {

		lua_reg("entityId", &entity);
		lua_reg("facetId", &facetId);		
	}

	std::string PlaySoundEvent::getEventTypeNameImpl() {
		return "PlaySoundEvent";
	}

	bool PlaySoundEvent::createFromLua(LuaState& lua) {		

		auto pse = PlaySoundEvent{};
		pse.fromLua(lua);
		single<EventProcessor>().process(pse);
		return false;
	}




} //end namespace core