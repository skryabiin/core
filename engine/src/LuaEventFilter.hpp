#ifndef CORE_LUA_EVENT_FILTER_HPP
#define CORE_LUA_EVENT_FILTER_HPP

#include "LuaTable.hpp"
#include "Entity.hpp"
namespace core {


	struct LuaEventFilter : public LuaTable, public equal_comparable<LuaEventFilter> {

		LuaEventFilter() {
			lua_reg("participant", &participant);
			lua_reg("eventTypeName", &eventTypeName);
		}

		Entity participant;
		std::string eventTypeName;
		int callback;

	};

} //end namespace core

#endif