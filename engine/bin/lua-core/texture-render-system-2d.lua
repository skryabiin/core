TextureRenderSystem2d = System:new()

function TextureRenderSystem2d:new(systemName, layerId, o)
    o = o or {}
    o = self:create(self:type(), systemName, layerId)
    Core.createSystem(self:type(), systemName, layerId)
    return o
end

function TextureRenderSystem2d:type()
    return "TextureRenderSystem2d"
end