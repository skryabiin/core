TextRenderSystem2d = System:new()

function TextRenderSystem2d:new(systemName, layerId, o)
    o = o or {}
    o = self:create(self:type(), systemName, layerId)
    Core.createSystem(self:type(), systemName, layerId)
    return o
end

function TextRenderSystem2d:type()
    return "TextRenderSystem2d"
end