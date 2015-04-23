#ifndef CORE_ANIMATION_CHANGE_EVENT_HPP
#define CORE_ANIMATION_CHANGE_EVENT_HPP

#include "Core.hpp"
#include "Event.hpp"
#include "LuaUtil.hpp"
#include "LuaState.hpp"

namespace core {

	struct AnimationChangeEvent : public Event<AnimationChangeEvent> {

		AnimationChangeEvent();

		std::string nextAnimation;
		bool endImmediate;
		Entity entity;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);


	};

} //end namespace core


#endif