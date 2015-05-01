#ifndef CORE_ACCELERATION_CHANGE_EVENT_HPP
#define CORE_ACCELERATION_CHANGE_EVENT_HPP


#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {

	
	struct AccelerationChangeEvent : public Event<AccelerationChangeEvent> {



		AccelerationChangeEvent();
		

		bool additive;

		int axes;

		LuaVec2 accelerationChange;

		Entity entity;

		static std::string getEventTypeNameImpl();
		
		static bool createFromLua(LuaState& lua);
	};



} //end namespace core

#endif