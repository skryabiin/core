#ifndef CORE_ANIMATION_SYSTEM_HPP
#define CORE_ANIMATION_SYSTEM_HPP

#include <string>

#include "System.hpp"
#include "UpdateableSystem.hpp"
#include "AnimationFacet.hpp"
#include "Animation.hpp"
#include "AnimationChangeEvent.hpp"
#include "PositionChangeEvent.hpp"
#include "RenderableSystem2d.hpp"
#include "VisualFacet.hpp"
#include "FacetDimensionQuery.hpp"
#include "FacetVector.hpp"

namespace core {





	#pragma warning( disable : 4250)

	struct ColorStateChange;

	class AnimationSystem2d : public RenderableSystem2d, public UpdateableSystem, public EventListener<AnimationChangeEvent> {

	public:

		AnimationSystem2d();		

		AnimationFacet* createAnimationFacet(Entity& e);

		virtual std::vector<Facet*> getFacets(Entity& e) override;

		virtual void updateImpl(float dt, RuntimeContext& context) override;		

		virtual void updateDrawablePosition(VisualFacet* vfacet);

		bool handleEvent(AnimationChangeEvent& animationChange);		

		bool handleEvent(FacetDimensionQuery& e);

		using RenderableSystem2d::handleEvent;
		
		virtual bool createImpl() override;
		virtual bool initializeImpl() override;
		virtual bool resetImpl() override;
		virtual bool destroyImpl() override;

		virtual void destroyFacets(Entity& entity) override;

		static int createFacet_bind(LuaState& lua);

		virtual ~AnimationSystem2d() {};

	private:		

		FacetVector<AnimationFacet> _facets;

		std::map<Entity, AnimationFacet*> _movingAnimations;

	};

} //end namespace core


#endif