
Sounds = {}

function Sounds.loadSound(v)
Console.info("Loading sound: '", v.name, "', ", v.path) 
	loadSound_bind(v.name, v.path, true)
end

function Sounds.load(sounds)
Console.info("Loading sounds...")
Sounds.sounds = sounds
	for i, v in ipairs(sounds) do Sounds.loadSound(v) end
end
