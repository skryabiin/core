TextureRenderSystem2d = System:new()

function TextureRenderSystem2d:new(systemName, layerId, camera, o)
    o = o or {}
    o = self:create(self:type(), systemName, layerId)
    Core.createSystem(self:type(), systemName, layerId)
    o:setCamera(camera)
    return o
end

function TextureRenderSystem2d:type()
    return "TextureRenderSystem2d"
end

function TextureRenderSystem2d:camera()
    return self._camera
end

function TextureRenderSystem2d:setCamera(camera)
    self._camera = camera or "world"
    setCamera_bind(self:name(), self:camera())
end

function TextureRenderSystem2d:isRenderable()
    return true
end