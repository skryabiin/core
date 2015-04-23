#ifndef CORE_PARTICLE_FIELD_HPP
#define CORE_PARTICLE_FIELD_HPP

#include <vector>
#include <functional>
#include "PositionChangeEvent.hpp"
#include "Templates.hpp"

namespace core {


	
	class ParticleFieldBase : public updateable<ParticleFieldBase, bool, int>{

	public:

		virtual void reset(unsigned numParticles) = 0;

		virtual bool updateImpl(int dt) = 0;
		
		virtual void updateDrawPosition(PositionChangeEvent& e) {};

		virtual ~ParticleFieldBase() {};


		virtual void TEMP_render(SDL_Renderer* renderer) {};

	private:
	};


	template <typename Particle_type>
	class ParticleField : public ParticleFieldBase  {

	public:
	
		virtual void reset(unsigned numParticles) override {
			_particles = std::vector<Particle_type> {numParticles, Particle_type{}};
			_fillFunction(this);
		}

		virtual bool updateImpl(int dt) override {
			
			auto particleIsActive = true;
			for (auto it = std::begin(_particles); it != std::end(_particles); ++it) {
				particleIsActive = _updateFunction(this, *it, dt);
				if (!particleIsActive) _generateFunction(this, *it);
			}

			return true;
		}

		void setFillFunction(std::function<void(ParticleField<Particle_type>*)> fillFunction) {
			_fillFunction = fillFunction;
		}

		void setGenerateFunction(std::function<void(ParticleField<Particle_type>*, Particle_type&)> generateFunction) {
			_generateFunction = generateFunction;
		}

		void setUpdateFunction(std::function<bool(ParticleField<Particle_type>*, Particle_type&, int)> updateFunction) {

			_updateFunction = updateFunction;
		}

		std::vector<Particle_type>& particles() {

			return _particles;
		}

		virtual void updateDrawPosition(PositionChangeEvent& positionChange) {};



		virtual ~ParticleField() {
			_particles.clear();
		}

	protected:


		std::vector<Particle_type> _particles;
		std::function<void(ParticleField<Particle_type>*)> _fillFunction;
		std::function<bool(ParticleField<Particle_type>*, Particle_type&, int)> _updateFunction;
		std::function<void(ParticleField<Particle_type>*, Particle_type&)> _generateFunction;


	};




} //end namespace core


#endif