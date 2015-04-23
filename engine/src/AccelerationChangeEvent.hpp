#ifndef CORE_ACCELERATION_CHANGE_EVENT_HPP
#define CORE_ACCELERATION_CHANGE_EVENT_HPP


#include "Event.hpp"

namespace core {

	
	struct AccelerationChangeEvent : public Event<AccelerationChangeEvent> {



		AccelerationChangeEvent()  {

			lua_reg("additive", &additive);
			lua_reg("axes", &axes);
			lua_reg("entityId", &entity);
		}
		

		bool additive;

		int axes;

		LuaVec2 accelerationChange;

		Entity entity;

		static std::string getEventTypeNameImpl() {

			return "AccelerationChangeEvent";
		}
	};



} //end namespace core

#endif