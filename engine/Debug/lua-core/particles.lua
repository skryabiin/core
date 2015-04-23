

ParticleEffects = {}


function ParticleEffects.load(particles) 
	Console.info("Loading particle effects...")
	ParticleEffects.particleEffects = particles
	for i, v in pairs(particles) do
		v.name = i
		loadParticleEffect_bind(v)
	end
end
		