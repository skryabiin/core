#ifndef CORE_PARTICLE_SYSTEM_2D_HPP
#define CORE_PARTICLE_SYSTEM_2D_HPP

#include "UpdateableSystem.hpp"
#include "RenderableSystem2d.hpp"
#include "ParticleFacet.hpp"
#include "VisualFacet.hpp"

namespace core {


	class ParticleSystem2d : public UpdateableSystem, public RenderableSystem2d {

	public:

		ParticleFacet& createParticleFacet(Entity& e);		

		virtual void updateImpl(float dt, RuntimeContext& context) override;

		using RenderableSystem2d::handleEvent;

		virtual bool createImpl() override;

		virtual bool initializeImpl() override;

		virtual bool resetImpl() override;

		virtual bool destroyImpl() override;

		virtual void destroyFacets(Entity& e) override;

		virtual void updateDrawablePosition(VisualFacet* vfacet);

		void renderFacet(SDL_Renderer* renderer, Facet* facet);

		static int createFacet_bind(LuaState& lua);

		virtual ~ParticleSystem2d() {};

	private:

		std::vector<ParticleFacet> _facets;


	};


} //end namespace core

#endif