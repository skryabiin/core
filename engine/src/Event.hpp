#ifndef CORE_EVENT_HPP
#define CORE_EVENT_HPP

#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <map>
#include <SDL.h>
#include "lua.hpp"
#include "Templates.hpp"
#include "Config.hpp"
#include "Entity.hpp"
//#include "EventListener.hpp"
#include "LuaState.hpp"
#include "LuaTable.hpp"
#include "EventProcessor.hpp"

namespace core {







	template <typename T>
	struct Event : public LuaTable {

		Event() {
			_eventTypeName = T::getEventTypeNameImpl();
			lua_reg("typeName", &_eventTypeName);
		}

		std::string getEventTypeName() {
			return _eventTypeName;
		}

		EventListener<T>* originator;

		std::string _eventTypeName;

	};


   



} //end namespace core

#endif
