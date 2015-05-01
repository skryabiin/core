#ifndef CORE_SCALE_CHANGE_EVENT_HPP
#define CORE_SCALE_CHANGE_EVENT_HPP

#include "Core.hpp"
#include "EventProcessor.hpp"
#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {


	struct ScaleChangeEvent : public Event<ScaleChangeEvent> {

		ScaleChangeEvent();

		Entity entity;
		LuaVec2 scale;
		int facetId;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);
	};



} //end namespace core

#endif