#ifndef CORE_PLAY_SOUND_EVENT_HPP
#define CORE_PLAY_SOUND_EVENT_HPP

#include "Core.hpp"
#include "Event.hpp"
#include "LuaUtil.hpp"
#include "LuaState.hpp"

namespace core {

	struct PlaySoundEvent : public Event<PlaySoundEvent> {

		PlaySoundEvent();

		Entity entity;
		int facetId;		

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);


	};
} //end namespace core

#endif