#ifndef CORE_AUDIO_FACET_HPP
#define CORE_AUDIO_FACET_HPP

#include "Facet.hpp"
#include "Sound.hpp"

namespace core {

	struct AudioFacet : public Facet {

		Sound* sound;
		float baseVolume;

	};

} //end namespace core

#endif