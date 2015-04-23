#ifndef CORE_ANIMATION_SYSTEM_HPP
#define CORE_ANIMATION_SYSTEM_HPP

#include <string>

#include "System.hpp"
#include "UpdateableSystem.hpp"
#include "AnimationFacet.hpp"
#include "Animation.hpp"
#include "AnimationChangeEvent.hpp"
#include "PositionChangeEvent.hpp"
#include "EventFilter.hpp"
#include "RenderableSystem2d.hpp"

namespace core {






	struct ColorStateChange;

	class AnimationSystem2d : public RenderableSystem2d, public UpdateableSystem {

	public:

		AnimationSystem2d();

		AnimationFacet& createAnimationFacet(Entity& e);

		virtual std::vector<Facet*> getFacets(Entity& e) override;

		virtual void updateImpl(RuntimeContext& context) override;		

		void handleAnimationChange(AnimationChangeEvent& animationChange);

		virtual void updateDrawPosition(PositionChangeEvent& positionChange) override;

		virtual void renderFacet(SDL_Renderer* renderer, Facet* facet) {};

		virtual void handleFacetPauseEvent(FacetPauseEvent& pauseEvent) override;
		virtual InitStatus initializeImpl() override;

		virtual InitStatus resetImpl() override;
		
		virtual void destroyFacets(Entity& entity) override;

		static int createFacet_bind(LuaState& lua);

	private:

		EventFilter<AnimationChangeEvent> _animationChangeFilter;

		std::vector<AnimationFacet> _animationFacets;

		std::map<Entity, AnimationFacet*> _movingAnimations;

	};

} //end namespace core


#endif