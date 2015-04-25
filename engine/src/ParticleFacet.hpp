#ifndef CORE_PARTICLE_FACET_HPP
#define CORE_PARTICLE_FACET_HPP

#include <vector>
#include <SDL.h>
#include "Facet.hpp"
#include "ParticleField.hpp"
#include "Drawable.hpp"

namespace core {


	
	struct ParticleFacet : public Facet {

		ParticleFacet() {}

		ParticleFieldBase* particleField; 

		Drawable drawable;

	};
	


} //end namespace core

#endif