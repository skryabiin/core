BasicPositionSystem2d = System:new()

function BasicPositionSystem2d:new(systemName, o)
    o = o or {}
    o = self:create(self:type(), systemName)
    Core.createSystem(self:type(), systemName, layerId)
    return o
end

function BasicPositionSystem2d:type()
    return "BasicPositionSystem2d"
end