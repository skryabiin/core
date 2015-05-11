#ifndef CORE_TRANSITION_REGISTRY
#define CORE_TRANSITION_REGISTRY

#include <map>
#include <functional>
#include "LuaState.hpp"
#include "Transition.hpp"

namespace core {


	struct TransitionRegistry {

		static std::map<std::string, std::function<TransitionBase*(LuaState&)>>& registeredLuaHandlers();
	};


	template <typename TRANSITION_TYPE>
	struct TransitionRegistration {

		TransitionRegistration() {

			TransitionRegistry::registeredLuaHandlers()[TRANSITION_TYPE::getTransitionTypeNameImpl()] = &TRANSITION_TYPE::createFromLua;
		}

	};


} //end namespace core


#endif