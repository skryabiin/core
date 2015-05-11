#include "TransitionRegistry.hpp"


#include "FadeTransition.hpp"
#include "RendererFadeTransition.hpp"

namespace core {


	std::map<std::string, std::function<TransitionBase*(LuaState&)>>& TransitionRegistry::registeredLuaHandlers() {

		static std::map<std::string, std::function<TransitionBase*(LuaState&)>> _registeredLuaHandlers;
		return _registeredLuaHandlers;
	}

	
	TransitionRegistration<FadeTransition> fadeTransitionReg{};
	TransitionRegistration<RendererFadeTransition> rendererFadeTransitionReg{};

}