#ifndef CORE_PHYSICS_HPP
#define CORE_PHYSICS_HPP


#include "LuaUtil.hpp"
#include "Event.hpp"
#include "Entity.hpp"
#include "EventProcessor.hpp"

namespace core {


	
	//EventProcessor::EventRegistration<AccelerationChangeEvent> accelChangeEventReg{};

	struct VelocityChangeEvent : public Event<VelocityChangeEvent> {


		VelocityChangeEvent() 
		{
			lua_reg("entityId", &entity);
			lua_reg("additive", &additive);
			lua_reg("velocityChange", &velocityChange);
			lua_reg("axes", &axes);
		}

		bool additive;

		int axes;

		LuaVec2 velocityChange;

		Entity entity;

		static std::string getEventTypeNameImpl() {

			return "VelocityChangeEvent";
		}

		static bool createFromLua(LuaState& lua) {
			auto vce = VelocityChangeEvent{};
			vce.setOriginator(&single<EventProcessor>().luaDummyListener);
			vce.fromLua(lua);
			single<EventProcessor>().process(vce);
			return false;
		}

	};



} 

#endif