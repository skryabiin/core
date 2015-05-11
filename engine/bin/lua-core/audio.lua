
Audio = {}

function Audio.loadSound(v)
Console.info("Loading sound: '", v.name, "', ", v.path) 
	loadSound_bind(v.name, v.path, true)
end

function Audio.load(sounds)
Console.info("Loading sounds...")
Audio.sounds = sounds
	for i, v in ipairs(sounds) do Audio.loadSound(v) end
end

function Audio.setGlobalVolumeLevel(level)
    setGlobalVolumeLevel_bind(level)
end

function Audio.getGlobalVolmeLevel()
    return getGlobalVolmeLevel_bind(level)
end