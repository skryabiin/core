#include "AnimationChangeEvent.hpp"
#include "Console.hpp"
#include "EventProcessor.hpp"

namespace core {


	AnimationChangeEvent::AnimationChangeEvent() {
		
		lua_reg("entityId", &entity);
		lua_reg("nextAnimation", &nextAnimation);
		lua_reg("endImmediate", &endImmediate);
	}

	std::string AnimationChangeEvent::getEventTypeNameImpl() {
		return "AnimationChangeEvent";
	}

	bool AnimationChangeEvent::createFromLua(LuaState& lua) {
		debug("Animation change event received.");

		auto ace = AnimationChangeEvent{};
		ace.fromLua(lua);
		single<EventProcessor>().process(ace);
		return false;
	}


} //end namespace core