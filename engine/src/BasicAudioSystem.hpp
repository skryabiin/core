#ifndef CORE_BASIC_AUDIO_SYSTEM_HPP
#define CORE_BASIC_AUDIO_SYSTEM_HPP

#include "System.hpp"
#include "PlaySoundEvent.hpp"
#include "AudioFacet.hpp"
#include "VolumeChangeEvent.hpp"
namespace core {


	class BasicAudioSystem : public System, public EventListener<PlaySoundEvent>, public EventListener<VolumeChangeEvent> {
	
	public:

		virtual bool createImpl();
		virtual bool initializeImpl();
		virtual bool resetImpl();
		virtual bool destroyImpl();

		virtual void pauseImpl() {};
		virtual void resumeImpl() {};

		virtual bool handleEvent(FacetPauseEvent& e);

		bool handleEvent(PlaySoundEvent& e);

		bool handleEvent(VolumeChangeEvent& e);

		virtual std::vector<Facet*> getFacets(Entity& e);

		virtual void destroyFacets(Entity& e);

		int createFacet(Entity& e, std::string soundName, float baseVolume);

		void updateFacet(Entity& e, int facetId, std::string soundName);

		static int updateFacet_bind(LuaState& lua);

		static int createFacet_bind(LuaState& lua);

	private:

		std::vector<AudioFacet> _audioFacets;
	};


} //end namespace core

#endif