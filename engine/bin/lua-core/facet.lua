
Facet = {}



function Facet:create(entity, system, o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self	
	if entity then
		o._of = entity		
	end
    if system then
        o._system = system
    end
	return o
end

function Facet:new(entity, system, o)
	return Facet:create(entity, system, o)
end

function Facet:id()
	return self._id or -1
end

function Facet:isRenderableFacet()
	return false
end

function Facet:type()
	return "Facet"
end

function Facet:dimensions()
	local dimensionQueryEvent = {
		typeName = "FacetDimensionQuery",
		entityId = self:of():id(),
		facetId = self:id(),
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
	Console.info("facet is: " .. self:id())
	Console.info("entity is: " .. self:of():id())
	local facetPauseEvent = {
			typeName = "FacetPauseEvent",
			entityId = self:of():id(),
			paused = paused,
			facetId = self:id()}	
	Console.info("facet is: " .. facetPauseEvent.facetId)
	Console.info("entity is: " .. facetPauseEvent.entityId)			
	EventProcessor.process(facetPauseEvent)
	self.paused = paused
end

function Facet:system()
	return self._system
end

function Facet:setSystem(system)
	self._system = system
end

function Facet:setOf(entity)
	self._of = entity
end

function Facet:of()
	return self._of
end
	
function Facet:isCoreFacet()
	return self:id() ~= -1
end

function Facet:createFacetInCore()
end

function Facet:updateFacetInCore()
end

