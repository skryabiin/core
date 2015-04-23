#ifndef CORE_ENTITY_POSITION_QUERY_HPP
#define CORE_ENTITY_POSITION_QUERY_HPP

#include "Core.hpp"
#include "LuaVector.hpp"
#include "Event.hpp"
#include "Entity.hpp"

namespace core {

	struct EntityPositionQuery : public Event<EntityPositionQuery> {

		EntityPositionQuery();

		Entity entity;
		LuaPixel position;
		LuaDimension dimensions;
		LuaVec2 orientation; 
		bool found;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

	};

	

} //end namespace core

#endif