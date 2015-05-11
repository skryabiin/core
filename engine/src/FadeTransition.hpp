#ifndef CORE_FADE_TRANSITION_HPP
#define CORE_FADE_TRANSITION_HPP

#include "Transition.hpp"
#include "Color.hpp"
#include "ColorModulationEvent.hpp"
#include "Interpolation.hpp"
namespace core {


	struct FadeTransition : public Transition<FadeTransition> {

		FadeTransition();

		Entity entity;
		int facetId;
		float duration;
		LuaColorTransform beginLuaTransform;
		LuaColorTransform endLuaTransform;				
		ColorModulationEvent publishEvent;
		Interpolation* interpolation;

		virtual bool updateImpl(float& dt);

		static std::string getTransitionTypeNameImpl();

		static TransitionBase* createFromLua(LuaState& lua);

		virtual ~FadeTransition();

	private:		
		ColorTransform _beginTransform;
		ColorTransform _endTransform;
		ColorTransform _diffTransform;
	};


} //end namespace core
#endif