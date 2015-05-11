#include "BasicAudioSystem.hpp"
#include "AudioManager.hpp"

namespace core {


	bool BasicAudioSystem::createImpl() {
		if (!System::createImpl()) {
			return false;
		}
		return true;
	}

	bool BasicAudioSystem::initializeImpl() {
		if (!System::initializeImpl()) {
			return false;
		}
		return true;
	}

	bool BasicAudioSystem::resetImpl() {
		_audioFacets.clear();
		return System::resetImpl();
	}

	bool BasicAudioSystem::destroyImpl() {
		return System::destroyImpl();
	}

	bool BasicAudioSystem::handleEvent(PlaySoundEvent& e) {

		for (auto& facet : _audioFacets) {
			if (e.entity == facet.of()) {
				if (e.facetId == -1 || e.facetId == facet.id()) {
					single<AudioManager>().playSound(facet.sound, facet.baseVolume);
					if (e.facetId != -1) return false;
				}
			}
		}		
		return true;
	}

	bool BasicAudioSystem::handleEvent(VolumeChangeEvent& e) {
		for (auto& facet : _audioFacets) {
			if (e.entity == facet.of()) {
				if (e.facetId != -1 || e.facetId == facet.id()) {
					facet.baseVolume = e.volume;
					if (e.facetId != -1) return false;
				}
			}
		}
		return true;
	}

	bool BasicAudioSystem::handleEvent(FacetPauseEvent& e) {

		auto facets = getFacets(e.entity);
		for (auto& facet : facets) {			
				if (e.facetId == -1 || e.facetId == facet->id()) {
					if (e.paused) {
						facet->pause();
					}
					else {
						facet->resume();
					}
				}
				if (e.facetId != -1) {
					return false;
				}						
		}
		return true;
	}

	std::vector<Facet*> BasicAudioSystem::getFacets(Entity& e) {
		auto out = std::vector<Facet*>{};

		for (auto& facet : _audioFacets) {
			if (facet.of() == e) {
				out.push_back(&facet);
			}
		}
		return out;
	}

	void BasicAudioSystem::destroyFacets(Entity& e) {
		auto it = std::begin(_audioFacets);
		auto endIt = std::end(_audioFacets);

		while (it != endIt) {			
			if (it->of() == e) {				
				it = _audioFacets.erase(it);
				endIt = std::end(_audioFacets);
			}
			else {
				++it;
			}
		}
	}

	int BasicAudioSystem::createFacet(Entity& e, std::string soundName, float baseVolume) {

		auto audio = AudioFacet{};
		audio.setOf(e);
		audio.baseVolume = baseVolume;
		audio.sound = single<AudioManager>().getSound(soundName);
		_audioFacets.push_back(audio);
		return audio.id();

	}

	void BasicAudioSystem::updateFacet(Entity& e, int facetId, std::string soundName) {
		for (auto& facet : _audioFacets) {
			if (facet.of() == e) {
				if (facetId == -1 || facet.id() == facetId) {
					facet.sound = single<AudioManager>().getSound(soundName);					
					if (facetId == -1) return;
				}
			}
		}
	}

	int BasicAudioSystem::createFacet_bind(LuaState& lua) {

		auto systemName = lua.pullStack<std::string>(1);
		auto entity = lua.pullStack<int>(2);

		auto system = single<Core>().getSystemByName<BasicAudioSystem>(systemName);

		if (system != nullptr) {
			std::string soundName = lua["soundName"];
			float volume = lua["volume"];
			int facetId = system->createFacet(entity, soundName, volume);
			lua.pushStack(facetId);
			return 1;
		}
		else {
			return 0;
		}
	}

	int BasicAudioSystem::updateFacet_bind(LuaState& lua) {
		auto systemName = lua.pullStack<std::string>(1);
		auto entity = lua.pullStack<int>(2);

		auto system = single<Core>().getSystemByName<BasicAudioSystem>(systemName);

		if (system != nullptr) {
			std::string soundName = lua["soundName"];
			int facetId = lua["facetId"];
			system->updateFacet(entity, facetId, soundName);
		}
		else {
			warn("Attempting to update facet in non-existant system '", systemName, "'.");
		}
		return 0;
	}
} //end namespace core