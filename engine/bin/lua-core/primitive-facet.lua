PrimitiveFacet = Facet:new()

function PrimitiveFacet:new(entity, o)
	o = o or {}
	return self:create(entity, o)
end

function PrimitiveFacet:getType()
	return "PrimitiveFacet"
end

function PrimitiveFacet:isRenderableFacet()
	return true
end

function PrimitiveFacet:getOffset()
	return self.offset or {0,0}
end
	
	
function PrimitiveFacet:setOffset(offset)
	self.offset = offset
	if self:isCoreFacet() then	
		local offsetChange = {
			typeName = "OffsetChangeEvent",
			entityId = self.of:getId(),
			offset = offset
		}
		EventProcessor.process(offsetChange)	
	end
end

function PrimitiveFacet:setScale(scale)
	self.scale = scale
    if self:isCoreFacet() then
        local scaleChange = {
            typeName = "ScaleChangeEvent",
            entityId = self.offset:getId(),
            scale = self.scale,
            facetId = self:getId()
        }
        EventProcessor.process(scaleChange)
    end
end

function PrimitiveFacet:getScale()
	return self.scale or {1,1}
end

function PrimitiveFacet:getColor()
	return self.color or {255,255,255,255}
end

function PrimitiveFacet:setColor(color)
	self.color = color
    if self:isCoreFacet() then	
		local colorChange = {
			typeName = "ColorChangeEvent",
			entityId = self.of:getId(),            
            color = self.color,
            facetId = self.getId()
		}
		EventProcessor.process(colorChange)	
	end
end


function PrimitiveFacet:createFacetInCore()
	if not self:isCoreFacet() then
		local primitiveFacet = {
			position = self.of:getPosition(),
			offset = self:getOffset(),
            scale = self:getScale(),
			color = self:getColor()
		}
			
		self.id = addPrimitiveFacet_bind(self.systemName, self.of:getId(), primitiveFacet)		
		self:setIsCoreFacet(true)
	end
end
		
