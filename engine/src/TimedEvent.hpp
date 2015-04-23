#ifndef CORE_TIMED_EVENT_HPP
#define CORE_TIMED_EVENT_HPP

#include <string>

#include "Event.hpp"

namespace core {


	struct TimedEvent : public Event<TimedEvent> {

		TimedEvent() {
			lua_reg("luaCallbackRef", &luaCallbackRef);
			lua_reg("interval", &interval);
			callbackThreadState = nullptr;
		}

		std::string eventName;

		int interval;
		
		Entity entity;

		int luaCallbackRef;		

		lua_State* callbackThreadState;

		static std::string getEventTypeNameImpl() {
			return "TimedEvent";
		}

	};




} //end namespace core

#endif