#ifndef CORE_DEBUG_EVENT_HPP
#define CORE_DEBUG_EVENT_HPP

#include "Event.hpp"

namespace core {

	struct DebugEvent : public Event<DebugEvent> {

		static std::string getEventTypeNameImpl() {

			return "DebugEvent";
		}
	};



	
} //end namespace core

#endif