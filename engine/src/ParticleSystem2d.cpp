#include "ParticleSystem2d.hpp"
#include "Templates.hpp"
#include "Core.hpp"

namespace core {

	ParticleFacet& ParticleSystem2d::createParticleFacet(Entity& e) {

		auto facet = ParticleFacet{};
		_facets.push_back(std::move(facet));
		auto& pf = _facets.back();
		pf.setOf(e);				
		pf.drawable.facet = &pf;
		pf.drawable.zIndex = -10;		
		//pf.drawable.id = single<Renderer>().createDrawable(pf.drawable);
		return pf;
	}

	void ParticleSystem2d::updateImpl(float dt, RuntimeContext& context) {

		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {
			if (it->isPaused()) continue;
			it->particleField->update(context.dt);
		}

	}

	void ParticleSystem2d::updateDrawablePosition(VisualFacet* vfacet) {

	}

	void ParticleSystem2d::renderFacet(SDL_Renderer* renderer, Facet* facet) {
		static_cast<ParticleFacet*>(facet)->particleField->TEMP_render(renderer);		
	}

	bool ParticleSystem2d::createImpl() {
		return RenderableSystem2d::createImpl();
	}

	bool ParticleSystem2d::initializeImpl() {

		return RenderableSystem2d::initializeImpl();
	}

	bool ParticleSystem2d::resetImpl() {

		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::DESTROY_DRAWABLE;
		dc.layerId = _drawableLayerId;
		for (auto& facet : _facets) {
			dc.facetId = facet.id();
			single<Renderer>().applyDrawableChange(dc);
		}

		_facets.clear();
		_facets.shrink_to_fit();

		return RenderableSystem2d::resetImpl();

	}

	bool ParticleSystem2d::destroyImpl() {
		return RenderableSystem2d::destroyImpl();
	}

	void ParticleSystem2d::destroyFacets(Entity& e) {


	}

	int ParticleSystem2d::createFacet_bind(LuaState& lua) {

		auto systemName = lua.pullStack<std::string>(1);
		auto entity = lua.pullStack<int>(2);
		std::string defName = lua["definition"];
		int numParticlesStart = lua["numParticlesStart"];
		int numParticlesMax = lua["numParticlesMax"];

		auto system = single<Core>().getSystemByName<ParticleSystem2d>(systemName);

		auto lpf = new LinearParticleField{};

		auto def = single<ResourceManager>().getParticleEffect(defName);

		lpf->init(numParticlesStart, numParticlesMax, def);
		lpf->reset(numParticlesStart);

		auto& facet = system->createParticleFacet(entity);

		facet.particleField = lpf;
		
		lua.pushStack(facet.id());

		return 1;
	}


}