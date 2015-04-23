#ifndef CORE_EVENT_LISTENER_HPP
#define CORE_EVENT_LISTENER_HPP

#include "Templates.hpp"


namespace core {

	class EventListener : public equal_comparable<EventListener> {

	};

	class LuaDummyListener : public EventListener {

	};



} //end namespace core


#endif