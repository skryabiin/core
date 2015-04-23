#ifndef CORE_ENTITY_LAYER_QUERY_HPP
#define CORE_ENTITY_LAYER_QUERY_HPP

#include "Event.hpp"

namespace core {


	struct EntityLayerQuery : public Event<EntityLayerQuery> {

		EntityLayerQuery();

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

		int layerId;
		Entity entity;
		bool found;




	};



} //end namespace core

#endif