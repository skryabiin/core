	
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
    self:setTexture()
end

function TextureFacet:setTextureCoordinates(sourceRect)
	self.coordinates = sourceRect
    self:setTexture()
end

function TextureFacet:setTexture()
    if self:isCoreFacet() then 
        local textureChange = {
            typeName = "TextureChangeEvent",
            entityId = self.of:getId(),            
            textureName = self.name,
            sourceTextureRect = self.coordinates,
            facetId = self:getId()
        }
        EventProcessor.process(textureChange)
    end
end


function TextureFacet:getTextureName()	
	return self.name
end

function TextureFacet:getTextureCoordinates()
	return self.coordinates or {0,0,0,0}
end

function TextureFacet:getOffset()
	return self.offset or {0,0}
end
	
function TextureFacet:setOffset(offset)
	self.offset = offset
	if self:isCoreFacet() then	
		local offsetChange = {
			typeName = "OffsetChangeEvent",
			entityId = self.of:getId(),
			offset = offset,
            facetId = self:getId()
		}
		EventProcessor.process(offsetChange)	
	end
end

function TextureFacet:scaleToEntity()

     

    if self:isCoreFacet() then
        local dimensions = self.of:getDimensions()         
        local myDimensions = self:getDimensions()
        local scale = self:getScale()
        local newScale = {dimensions[1] * scale[1] / myDimensions[1], dimensions[2] * scale[2] / myDimensions[2] }
        self:setScale(newScale)
    elseif self.coordinates and self.coordinates[3] ~= 0 and self.coordinates[4] ~= 0 then
        local dimensions = self.of:getDimensions() 
        local newScale = {dimensions[1] / self.coordinates[3], dimensions[2] / self.coordinates[3]}
        self:setScale(newScale)
    else
        self:setScale({1,1})
    end
       
end

function TextureFacet:setScale(scale)
	self.scale = scale
    if self:isCoreFacet() then
        local scaleChange = {
            typeName = "ScaleChangeEvent",
            entityId = self.of:getId(),
            scale = self:getScale(),
            facetId = self:getId()
        }
        EventProcessor.process(scaleChange)
    else
        debug("Setting scale for non-core facet")
    end
end

function TextureFacet:getScale()
	return self.scale or {1,1}
end

function TextureFacet:getColor()
	return self.color or {255,255,255,255}
end


function TextureFacet:createFacetInCore()
	if not self:isCoreFacet() then
		local facet = {
			source = self:getTextureCoordinates(),	
			textureName = self:getTextureName(),
			position = self.of:getPosition(),			
			scale = self:getScale(),
			offset = self:getOffset()
		}		
		self.id = addTextureFacet_bind(self.systemName, self.of:getId(), facet)
		
		self:setIsCoreFacet(true)
	end
	
end
