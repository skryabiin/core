#ifndef CORE_PARTICLE_SYSTEM_2D_HPP
#define CORE_PARTICLE_SYSTEM_2D_HPP

#include "UpdateableSystem.hpp"
#include "RenderableSystem2d.hpp"
#include "ParticleFacet.hpp"

namespace core {


	class ParticleSystem2d : public UpdateableSystem, public RenderableSystem2d {

	public:

		ParticleFacet& createParticleFacet(Entity& e);

		virtual void updateDrawPosition(PositionChangeEvent& positionChange) override;
		virtual void updateImpl(RuntimeContext& context) override;

		virtual InitStatus initializeImpl() override;

		virtual InitStatus resetImpl() override;

		virtual void destroyFacets(Entity& e) override;

		virtual void handleFacetPauseEvent(FacetPauseEvent& pauseEvent) override;

		void renderFacet(SDL_Renderer* renderer, Facet* facet);

		static int createFacet_bind(LuaState& lua);

	private:

		std::vector<ParticleFacet> _facets;


	};


} //end namespace core

#endif