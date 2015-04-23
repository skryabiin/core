#ifndef CORE_PARTICLE_FACET_HPP
#define CORE_PARTICLE_FACET_HPP

#include <vector>
#include <SDL.h>
#include "Facet.hpp"
#include "ParticleField.hpp"

namespace core {


	
	struct ParticleFacet : public Facet {

		ParticleFacet() {}

		ParticleFieldBase* particleField; 

		int drawableId;
	};
	


} //end namespace core

#endif