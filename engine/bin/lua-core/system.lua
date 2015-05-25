
System = {}



function System:create(systemType, systemName, layerId, o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self	
    if systemName then
        o:setName(systemName)
    end
    o._autoCreation = true
    --Core.createSystem(systemType, systemName, layerId)
	return o
end

function System:disableAutoCreation()
    self._autoCreation = false
end

function System:enableAutoCreation()
    self._autoCreation = true
end

function System:isAutoCreationEnabled()
    return self._autoCreation
end

function System:new(systemName, o)
	return System:create(entity, systemName, o)    
end

function System:type()
    return "System"
end

function System:setName(name)
    self._name = name
end

function System:name()
    return self._name
end

function System:isRenderable()
    return false
end

function System:camera()
    return nil
end

function System:setCamera(camera)

end