#include "RendererFadeTransition.hpp"
#include "Core.hpp"
#include "Renderer.hpp"

namespace core {



	RendererFadeTransition::RendererFadeTransition() {
		lua_reg("duration", &duration);		
		lua_reg("endTransform", &endLuaTransform);

	}

	RendererFadeTransition::~RendererFadeTransition() {
		delete interpolation;
	}

	std::string RendererFadeTransition::getTransitionTypeNameImpl() {
		return "RendererFadeTransition";
	}



	bool RendererFadeTransition::updateImpl(float& dt) {

		auto ivalue = interpolation->getValue(getElapsed());

		if (ivalue.done) {

			single<Core>().lua().call(finishedCallback);
			return true;
		}
		else {
			_currentTransform = _beginTransform + _diffTransform * ivalue.value;
			single<Renderer>().setGlobalColorModulation(_currentTransform);
			return false;
		}
	}

	TransitionBase* RendererFadeTransition::createFromLua(LuaState& lua) {

		auto fadeTransition = new RendererFadeTransition{};
		fadeTransition->fromLua(lua);
		
		fadeTransition->_beginTransform = single<Renderer>().getGlobalColorModulation();
		fadeTransition->_endTransform = fadeTransition->endLuaTransform.getColorTransform();
		fadeTransition->_diffTransform = fadeTransition->_endTransform - fadeTransition->_beginTransform;
		fadeTransition->interpolation = new LinearInterpolation{ fadeTransition->duration };

		return fadeTransition;

	}




} //end namespace core