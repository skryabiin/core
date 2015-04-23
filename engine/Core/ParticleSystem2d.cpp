#include "ParticleSystem2d.hpp"
#include "Templates.hpp"
#include "Core.hpp"

namespace core {

	ParticleFacet& ParticleSystem2d::createParticleFacet(Entity& e) {

		auto facet = ParticleFacet{};
		_facets.push_back(std::move(facet));
		auto& pf = _facets.back();
		pf.setOf(e);
		auto d = Drawable{};		
		d.facet = &pf;
		d.zIndex = -10;
		d.systemRendered = true;	
		d.system = this;
		pf.drawableId = single<Renderer>().createDrawable(d);
		return pf;
	}


	void ParticleSystem2d::handleFacetPauseEvent(FacetPauseEvent& pauseEvent) {		

		for (auto& facet : _facets) {
			if (facet.of() == pauseEvent.entity) {

				//doing all of the facets for this entity so continue
				if (pauseEvent.facetId == -1) {
					if (pauseEvent.paused) {
						facet.pause();
						single<Renderer>().pauseDrawable(facet.drawableId);
					}
					else {
						facet.resume();
						single<Renderer>().resumeDrawable(facet.drawableId);
					}
					continue;
				}

				//doing just the one facet for this entity so stop
				else if (pauseEvent.facetId == facet.id()) {
					if (pauseEvent.paused) {
						facet.pause();
						single<Renderer>().pauseDrawable(facet.drawableId);
					}
					else {
						facet.resume();
						single<Renderer>().resumeDrawable(facet.drawableId);
					}					
					break;
				}

			}
		}

	}


	void ParticleSystem2d::updateDrawPosition(PositionChangeEvent& positionChange) {

		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {

			it->particleField->updateDrawPosition(positionChange);
		}
	}

	void ParticleSystem2d::updateImpl(RuntimeContext& context) {

		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {
			if (it->isPaused()) continue;
			it->particleField->update(context.dt);
		}

	}

	void ParticleSystem2d::renderFacet(SDL_Renderer* renderer, Facet* facet) {
		static_cast<ParticleFacet*>(facet)->particleField->TEMP_render(renderer);		
	}


	InitStatus ParticleSystem2d::initializeImpl() {

		return RenderableSystem2d::initializeImpl();
	}

	InitStatus ParticleSystem2d::resetImpl() {

		for (auto& facet : _facets) {

			single<Renderer>().destroyDrawable(facet.drawableId);
		}

		_facets.clear();
		_facets.shrink_to_fit();

		return RenderableSystem2d::resetImpl();

	}

	void ParticleSystem2d::destroyFacets(Entity& e) {
		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {

			if (it->of() == e) {
				_facets.erase(it);
				--it;

			}

		}

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