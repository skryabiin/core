#ifndef CORE_LINEAR_PARTICLE_FIELD_HPP
#define CORE_LINEAR_PARTICLE_FIELD_HPP


#include <SDL.h>
#include "ParticleField.hpp"
#include "LuaTable.hpp"
#include "LuaVector.hpp"

namespace core {



	struct BasicParticle {

		BasicParticle() :		
		velocity{ 0.0f, 0.0f },
		accel{ 0.0f, 0.0f },
		color{ 0.0f, 0.0f, 0.0f, 0.0f },
		lifetime{ -1 },
		initialDelay{ 0 },
		active{ false }
		{ }


		SDL_Rect shape;
		Vec2 velocity;
		Vec2 accel;
		Color color;
		int lifetime;
		int initialDelay;
		bool active;
	};


	struct LinearParticleDef : public LuaTable {

		LinearParticleDef() {
			lua_reg("name", &name);
			lua_reg("numParticlesStart", &numParticlesStart);
			lua_reg("numParticlesMax", &numParticlesMax);
			lua_reg("startInMediaRes", &startInMediaRes);
			lua_reg("endOnZoneClip", &endOnZoneClip);
			lua_reg("fieldZone", &fieldZoneLua);
			lua_reg("initialDelayRange", &initialDelayRange);
			lua_reg("lifetimeRange", &lifetimeRange);
			lua_reg("velocityXRange", &velocityXRange);
			lua_reg("velocityYRange", &velocityYRange);
			lua_reg("accelXRange", &accelXRange);
			lua_reg("accelYRange", &accelYRange);
			lua_reg("colorRangeR", &colorRangeR);
			lua_reg("colorRangeG", &colorRangeG);
			lua_reg("colorRangeB", &colorRangeB);
			lua_reg("colorRangeA", &colorRangeA);
			lua_reg("fadeOut", &fadeOut);
			lua_reg("spawnZone", &spawnZoneLua);
			lua_reg("particleWidthRange", &particleWidthRange);
			lua_reg("particleHeightRange", &particleHeightRange);
		}


		std::string name;
	int numParticlesStart;
	int numParticlesMax;
		//start system with pre-spawned and advanced particles 
		// (i.e., a star field will have stars all over the screen
		// when it first starts, even though stars only spawn at the top)
		bool startInMediaRes;

		//removes particles if they clip the window
		//if the lifetime of the particle is infinite, this will default to true
		bool endOnZoneClip;

		//box where particles are bound, visually
		//if this is not set, full window is assumed
		LuaRect fieldZoneLua;
		SDL_Rect fieldZone;

		//range of initial delays for particles, in ms
		//initialDelayRange = {0, 1000}
		LuaRange<int> initialDelayRange;
		

		//range of lifetimes for particles, in ms
		//lifetimeRange = {2000, 3000} 
		LuaRange<int> lifetimeRange;
		
		LuaRange<float> velocityXRange;

		LuaRange<float> velocityYRange;

		LuaRange<float> accelXRange;

		LuaRange<float> accelYRange;

		//ranges for initial colors
		LuaRange<int> colorRangeR;
		LuaRange<int> colorRangeG;
		LuaRange<int> colorRangeB;
		LuaRange<int> colorRangeA;

		//adjusts alpha based on lifetime, ending at zero
		bool fadeOut;
		
		
		//box where particles will spawn randomly
		LuaRect spawnZoneLua;
		SDL_Rect spawnZone;

		//range of particle sizes
		//sizeRange = SDL_Rect { min_w, min_h, max_w, max_h }
		LuaRange<int> particleWidthRange;
		LuaRange<int> particleHeightRange;		

	};

	class LinearParticleField : public ParticleField<BasicParticle> {
	public:
		LinearParticleField();
		
		bool init(int numParticlesStart, int numParticlesMax, LinearParticleDef def);

		virtual void TEMP_render(SDL_Renderer* renderer);

		int screenHeight;
		int screenWidth;
		int numParticlesStart;
		int numParticlesMax;
		LinearParticleDef def;
	};
	
} //end namespace core


#endif