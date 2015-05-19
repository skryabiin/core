PrimitiveFacet = Facet:new()

function PrimitiveFacet:new(entity, system, o)
	o = o or {}
	return self:create(entity, system, o)
end

function PrimitiveFacet:type()
	return "PrimitiveFacet"
end

function PrimitiveFacet:isRenderableFacet()
	return true
end

function PrimitiveFacet:offset()
	return self._offset or {0,0}
end
	
	
function PrimitiveFacet:setOffset(offset)
	self._offset = offset
	if self:isCoreFacet() then	
		local offsetChange = {
			typeName = "OffsetChangeEvent",
			entityId = self:of():id(),
			offset = offset
		}
		EventProcessor.process(offsetChange)	
	end
end

function PrimitiveFacet:setScale(scale)
	self._scale = scale
    if self:isCoreFacet() then
        local scaleChange = {
            typeName = "ScaleChangeEvent",
            entityId = self:of():id(),
            scale = scale,
            facetId = self:id()
        }
        EventProcessor.process(scaleChange)
    end
end

function PrimitiveFacet:scale()
	return self._scale or {1,1}
end

function PrimitiveFacet:color()
	return self._color or {1,1,1,1}
end

function PrimitiveFacet:setColor(color)
	self._color = color
    if self:isCoreFacet() then	
		local colorChange = {
			typeName = "ColorChangeEvent",
			entityId = self:of():id(),            
            color = color,
            facetId = self:id()
		}
		EventProcessor.process(colorChange)	
	end
end


function PrimitiveFacet:createFacetInCore()
	if not self:isCoreFacet() then
		local primitiveFacet = {
			position = self:of():position(),
			offset = self:offset(),
            scale = self:scale(),
			color = self:color()
		}
			
		self._id = addPrimitiveFacet_bind(self:system():name(), self:of():id(), primitiveFacet)				
	end
end
		
