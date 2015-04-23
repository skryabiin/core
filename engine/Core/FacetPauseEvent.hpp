#ifndef CORE_FACET_PAUSE_EVENT_HPP
#define CORE_FACET_PAUSE_EVENT_HPP

#include "Event.hpp"

namespace core {

	struct FacetPauseEvent : public Event<FacetPauseEvent> {

		FacetPauseEvent();

		bool paused;
		Entity entity;
		long facetId;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

	};


} //end namespace core

#endif