#ifndef CORE_OFFSET_CHANGE_EVENT_HPP
#define CORE_OFFSET_CHANGE_EVENT_HPP

#include "Core.hpp"
#include "EventProcessor.hpp"
#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {

	struct OffsetChangeEvent : public Event<OffsetChangeEvent> {

		OffsetChangeEvent();

		Entity entity;
		LuaPixel offset;
		int facetId;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);


	};


} //end namespace core
#endif