
Facet = {}



function Facet:create(entity, systemName, o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self	
	if entity then
		o.of = entity		
	end
    if systemName then
        o:setSystemName(systemName)
    end
	return o
end

function Facet:new(entity, systemName, o)
	return Facet:create(entity, systemName, o)
end

function Facet:getId()
	return self.id or -1
end

function Facet:isRenderableFacet()
	return false
end

function Facet:getType()
	return "Facet"
end

function Facet:getDimensions()
	local dimensionQueryEvent = {
		typeName = "FacetDimensionQuery",
		entityId = self:getOf():getId(),
		facetId = self:getId(),
		dimensions = {0,0},
		found = false }
    
	dimensionQueryEvent = EventProcessor.process(dimensionQueryEvent)		
	return dimensionQueryEvent.dimensions

end

function Facet:isPaused()
	return self.paused
end

function Facet:pause()
	self:setPaused(true)
end

function Facet:resume()
	self:setPaused(false)
end

function Facet:togglePause()
	self:setPaused(not self.paused)
end

function Facet:setPaused(paused)
	Console.info("attempting to pause facet")
	Console.info("facet is: " .. self:getId())
	Console.info("entity is: " .. self.of:getId())
	local facetPauseEvent = {
			typeName = "FacetPauseEvent",
			entityId = self.of:getId(),
			paused = paused,
			facetId = self:getId()}	
	Console.info("facet is: " .. facetPauseEvent.facetId)
	Console.info("entity is: " .. facetPauseEvent.entityId)			
	EventProcessor.process(facetPauseEvent)
	self.paused = paused
end

function Facet:getSystemName()
	return self.systemName
end

function Facet:setSystemName(systemName)
	self.systemName = systemName
end

function Facet:setOf(entity)
	self.of = entity
end

function Facet:getOf()
	return self.of
end
	
function Facet:isCoreFacet()
	return self:getId() ~= -1
end

function Facet:createFacetInCore()
end

function Facet:updateFacetInCore()
end

