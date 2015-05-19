BasicAudioSystem = System:new()

function BasicAudioSystem:new(systemName, o)
    o = o or {}
    o = self:create(self:type(), systemName)
    Core.createSystem(self:type(), systemName, layerId)
    return o
end

function BasicAudioSystem:type()
    return "BasicAudioSystem"
end