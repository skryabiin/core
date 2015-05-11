#ifndef CORE_VOLUME_CHANGE_EVENT_HPP
#define CORE_VOLUME_CHANGE_EVENT_HPP

#include "Core.hpp"
#include "Event.hpp"
#include "LuaUtil.hpp"
#include "LuaState.hpp"

namespace core {


	struct VolumeChangeEvent : public Event<VolumeChangeEvent> {

		VolumeChangeEvent();

		Entity entity;
		int facetId;
		float volume;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);


	};

} //end namespace core

#endif