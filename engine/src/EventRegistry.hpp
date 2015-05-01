#ifndef CORE_EVENT_REGISTRY
#define CORE_EVENT_REGISTRY

#include "LuaState.hpp"
#include <functional>
#include <map>

namespace core {	

	struct EventRegistry {

		static std::map<std::string, std::function<bool(LuaState&)>>& registeredLuaHandlers();
	};


	template <typename EVENT_TYPE>
	struct EventRegistration {

		EventRegistration() {

			EventRegistry::registeredLuaHandlers()[EVENT_TYPE::getEventTypeNameImpl()] = &EVENT_TYPE::createFromLua;
		}

	};
} //end namespace core
#endif