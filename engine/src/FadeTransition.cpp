#include "FadeTransition.hpp"
#include "Core.hpp"

namespace core {


	FadeTransition::FadeTransition() {
		lua_reg("entityId", &entity);
		lua_reg("facetId", &facetId);
		lua_reg("duration", &duration);
		lua_reg("beginTransform", &beginLuaTransform);
		lua_reg("endTransform", &endLuaTransform);		
		
	}

	FadeTransition::~FadeTransition() {
		delete interpolation;
	}

	std::string FadeTransition::getTransitionTypeNameImpl() {
		return "FadeTransition";
	}



	bool FadeTransition::updateImpl(float& dt) {

		auto ivalue = interpolation->getValue(getElapsed());

		if (ivalue.done) {
			publishEvent.transform = _endTransform;
			//single<EventProcessor>().processTransitionEvent(publishEvent, true);
			single<Core>().lua().call(finishedCallback);
			return true;
		} else {
			publishEvent.transform = _beginTransform + _diffTransform * ivalue.value;
			single<EventProcessor>().processTransitionEvent(publishEvent, false);
			return false;
		}
	}

	TransitionBase* FadeTransition::createFromLua(LuaState& lua) {

		auto fadeTransition = new FadeTransition{};
		fadeTransition->fromLua(lua);

		fadeTransition->_beginTransform = fadeTransition->beginLuaTransform.getColorTransform();
		fadeTransition->_endTransform = fadeTransition->endLuaTransform.getColorTransform();
		fadeTransition->_diffTransform = fadeTransition->_endTransform - fadeTransition->_beginTransform;
		fadeTransition->interpolation = new LinearInterpolation{ fadeTransition->duration };


		fadeTransition->publishEvent.transform = fadeTransition->_beginTransform;
		fadeTransition->publishEvent.entity = fadeTransition->entity;
		fadeTransition->publishEvent.facetId = fadeTransition->facetId;
		single<EventProcessor>().process(fadeTransition->publishEvent);
		return fadeTransition;

	}



} //end namespace core