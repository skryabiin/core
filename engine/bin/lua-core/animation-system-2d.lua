AnimationSystem2d = System:new()

function AnimationSystem2d:new(systemName, layerId, camera, o)
    o = o or {}
    o = self:create(self:type(), systemName, layerId)
    Core.createSystem(self:type(), systemName, layerId)
    o:setCamera(camera)
    return o
end

function AnimationSystem2d:type()
    return "AnimationSystem2d"
end

function AnimationSystem2d:camera()
    return self._camera
end

function AnimationSystem2d:setCamera(camera)
    self._camera = camera or "world"
    setCamera_bind(self:name(), self:camera())
end

function AnimationSystem2d:isRenderable()
    return true
end