	
TextureFacet = Facet:new()

function TextureFacet:new(entity, o)
	o = o or {}
	return self:create(entity, o)
end

function TextureFacet:getType()
	return "TextureFacet"
end

function TextureFacet:isRenderableFacet()
	return true
end

function TextureFacet:setTextureName(textureName)

	self.name = textureName
	
end

function TextureFacet:setTextureCoordinates(sourceRect)
	self.coordinates = sourceRect		
end

function TextureFacet:getTextureName()	
	return self.name
end

function TextureFacet:getTextureCoordinates()
	return self.coordinates or {0,0,0,0}
end

function TextureFacet:getVisualOffset()
	return self.offset or {0,0}
end
	
	
function TextureFacet:setVisualOffset(offset)
	self.offset = offset
end

function TextureFacet:setVisualScale(scale)
	self.scale = scale
end

function TextureFacet:getVisualScale()
	return self.scale or {1,1}
end

function TextureFacet:getColor()
	return self.color or {255,255,255,255}
end

function TextureFacet:setColor(color)
	self.color = color	
	if self:isCoreFacet() then
		local colorChangeEvent = {
			typeName = "ColorChangeEvent",
			entityId = self.of:getId(),
			color = self:getColor(),
			doModulateColor = true,
			blendMode = 1,
			facetId = self:getId()
		}		
		EventProcessor.process(colorChangeEvent)		
	end
end

function TextureFacet:createFacetInCore()
	if self:isCoreFacet() == false then
		local facet = {
			source = self:getTextureCoordinates(),	
			textureName = self:getTextureName(),
			position = self.of:getPosition(),
			dimensions = self.of:getDimensions(),
			scale = self:getVisualScale(),
			offset = self:getVisualOffset()
		}		
		self.id = addTextureFacet_bind(self.systemName, self.of:getId(), facet)
		
		self:setIsCoreFacet(true)
	end
	
end
