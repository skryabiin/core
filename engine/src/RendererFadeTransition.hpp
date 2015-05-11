#ifndef CORE_RENDERER_FADE_TRANSITION_HPP
#define CORE_RENDERER_FADE_TRANSITION_HPP

#include "Transition.hpp"
#include "Interpolation.hpp"
#include "Color.hpp"
#include "LuaVector.hpp"

namespace core {


	struct RendererFadeTransition : public Transition<RendererFadeTransition> {
		RendererFadeTransition();

		float duration;

		LuaColorTransform endLuaTransform;
		Interpolation* interpolation;

		virtual bool updateImpl(float& dt);

		static std::string getTransitionTypeNameImpl();

		static TransitionBase* createFromLua(LuaState& lua);

		virtual ~RendererFadeTransition();

	private:
		ColorTransform _beginTransform;
		ColorTransform _endTransform;
		ColorTransform _diffTransform;
		ColorTransform _currentTransform;
	};




} //end namespace core
#endif