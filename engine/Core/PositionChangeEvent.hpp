#ifndef CORE_POSITION_CHANGE_EVENT_HPP
#define CORE_POSITION_CHANGE_EVENT_HPP

#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {

	struct PositionChangeEvent : public Event<PositionChangeEvent> {

		PositionChangeEvent();

		Entity entity;
		LuaPixel position;
		bool relative;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);
		
	};


} //end namespace core


#endif