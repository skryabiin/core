#ifndef CORE_COLLISION_EVENT_HPP
#define CORE_COLLISION_EVENT_HPP

#include "LuaUtil.hpp"
#include "LuaList.hpp"
#include "LuaVector.hpp"
#include "Event.hpp"



namespace core {
	

	struct ContactState : public LuaTable {
		
		ContactState(const ContactState& other) {
			int x = 0;
		}

		ContactState() :
		state{ -1 },
		blocking{ false }
		{
			lua_reg("entityId", &entity);
			lua_reg("contactNormal", &contactNormal);
			lua_reg("blocking", &blocking);
			lua_reg("state", &state);
		}


		Entity entity;
		LuaVec2 contactNormal;
		bool blocking;
		int state;


	};


	struct CollisionState : public Event<CollisionState> {

		CollisionState() {
			temp = &contactStates;

			lua_reg("entityId", &entity);
			lua_reg("contacts", &contactStates);
			lua_reg("newPosition", &newPosition);
			lua_reg("newVelocity", &newVelocity);
		};


		Entity entity;		
		LuaList<ContactState> contactStates;		
		LuaPixel newPosition;
		LuaVec2 newVelocity;
		void* temp;

		static std::string getEventTypeNameImpl() {
			return "CollisionEvent";
		}





	};


} //end namespace core


#endif