#include "LinearParticleField.hpp"
#include "Core.hpp"
#include "Math.hpp"

namespace core {



	LinearParticleField::LinearParticleField() {
		auto& lua = single<Core>().lua();
		screenWidth = lua("window")[1];
		screenHeight = lua("window")[2];
	}

	bool LinearParticleField::init(int numParticlesStart, int numParticlesMax, LinearParticleDef initDef) {

		def = initDef;

		def.fieldZone = def.fieldZoneLua.getRect();
		def.spawnZone = def.spawnZoneLua.getRect();
		setFillFunction(
			[&](ParticleField<BasicParticle>* lpf)->void {
			auto& particles = lpf->particles();
			for (int i = 0; i < def.numParticlesStart; i++) {

				auto bp = BasicParticle{};

				bp.initialDelay = randInRange(def.initialDelayRange[0], def.initialDelayRange[1]);
				bp.lifetime = randInRange(def.lifetimeRange[0], def.lifetimeRange[1]);
				int w = randInRange(def.particleWidthRange[0], def.particleWidthRange[1]);
				int h = randInRange(def.particleHeightRange[0], def.particleHeightRange[1]);
				int x = randInRange(def.spawnZone.x, def.spawnZone.x + def.spawnZone.w);
				int y = randInRange(def.spawnZone.y, def.spawnZone.y + def.spawnZone.h);
				bp.shape = SDL_Rect{ x, y, w, h };

				float vx = randInRange(def.velocityXRange[0], def.velocityXRange[1]);
				float vy = randInRange(def.velocityYRange[0], def.velocityYRange[1]);
				bp.velocity = Vec2{ vx, vy };

				float ax = randInRange(def.accelXRange[0], def.accelYRange[1]);
				float ay = randInRange(def.accelYRange[0], def.accelYRange[1]);
				bp.accel = Vec2{ ax, ay };

				bp.color.r = randInRange(def.colorRangeR[0], def.colorRangeR[1]);
				bp.color.g = randInRange(def.colorRangeG[0], def.colorRangeG[1]);
				bp.color.b = randInRange(def.colorRangeB[0], def.colorRangeB[1]);
				bp.color.a = randInRange(def.colorRangeA[0], def.colorRangeA[1]);

				bp.active = false;
			}
	});

		setUpdateFunction(
			[&](ParticleField<BasicParticle>* lpf, BasicParticle& p, int dt)->bool {
			if (p.initialDelay > 0) {
				p.initialDelay -= dt;
				return true;
			}

			p.active = true;

			if (p.lifetime != -1) {
				p.lifetime -= dt;
				if (p.lifetime <= 0) {
					p.active = false;
					return false;
				}
			}

			if (def.fadeOut) {
				float x1 = 255.0 * (1.0f - (def.lifetimeRange[1] - p.lifetime) * (1.0f / def.lifetimeRange[1]));
				int x2 = roundFloat(x1);
				p.color.a = x2;
			}

			p.shape.x += roundFloat(p.velocity.x * dt / 1000.0f);
			p.shape.y += roundFloat(p.velocity.y * dt / 1000.0f);
			p.velocity += (p.accel * (dt / 1000.0f));

			if (def.endOnZoneClip || p.lifetime == -1) {
				if (!SDL_HasIntersection(&p.shape, &def.fieldZone)) {
					return false;
				}
			}

			return true;

		});


		setGenerateFunction(
			[&](ParticleField<BasicParticle>* lpf, BasicParticle& bp)->void {			

			bp.initialDelay = randInRange(def.initialDelayRange[0], def.initialDelayRange[1]);
			bp.lifetime = randInRange(def.lifetimeRange[0], def.lifetimeRange[1]);
			int w = randInRange(def.particleWidthRange[0], def.particleWidthRange[1]);
			int h = randInRange(def.particleHeightRange[0], def.particleHeightRange[1]);
			int x = randInRange(def.spawnZone.x, def.spawnZone.x + def.spawnZone.w);
			int y = randInRange(def.spawnZone.y, def.spawnZone.y + def.spawnZone.h);
			bp.shape = SDL_Rect{ x, y, w, h };

			float vx = randInRange(def.velocityXRange[0], def.velocityXRange[1]);
			float vy = randInRange(def.velocityYRange[0], def.velocityYRange[1]);
			bp.velocity = Vec2{ vx, vy };

			float ax = randInRange(def.accelXRange[0], def.accelYRange[1]);
			float ay = randInRange(def.accelYRange[0], def.accelYRange[1]);
			bp.accel = Vec2{ ax, ay };

			bp.color.r = randInRange(def.colorRangeR[0], def.colorRangeR[1]);
			bp.color.g = randInRange(def.colorRangeG[0], def.colorRangeG[1]);
			bp.color.b = randInRange(def.colorRangeB[0], def.colorRangeB[1]);
			bp.color.a = randInRange(def.colorRangeA[0], def.colorRangeA[1]);

			bp.active = false;

		});
			
			

		return true;
	}


	void LinearParticleField::TEMP_render(SDL_Renderer* renderer) {

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		for (auto& p : _particles) {
			if (p.active) {
				SDL_SetRenderDrawColor(renderer, p.color.r, p.color.g, p.color.b, p.color.a);
				SDL_RenderFillRect(renderer, &p.shape);
			}
		}

	}

} //end namespace core
