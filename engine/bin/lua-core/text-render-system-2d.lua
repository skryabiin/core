TextRenderSystem2d = System:new()

function TextRenderSystem2d:new(systemName, layerId, camera, o)
    o = o or {}
    o = self:create(self:type(), systemName, layerId)
    Core.createSystem(self:type(), systemName, layerId)
    o:setCamera(camera)
    return o
end

function TextRenderSystem2d:type()
    return "TextRenderSystem2d"
end

function TextRenderSystem2d:camera()
    return self._camera
end

function TextRenderSystem2d:setCamera(camera)
    self._camera = camera or "world"
    setCamera_bind(self:name(), self:camera())
end


function TextRenderSystem2d:isRenderable()
    return true
end