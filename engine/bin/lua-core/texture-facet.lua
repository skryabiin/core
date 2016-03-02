	
TextureFacet = Facet:new()

function TextureFacet:new(entity, system, textureName, o)
	o = o or {}    
	o = self:create(entity, system, o)    
    if textureName then    
        o:setTextureName(textureName)
    end    
    return o
end

function TextureFacet:type()
	return "TextureFacet"
end

function TextureFacet:isRenderableFacet()
	return true
end

function TextureFacet:setTextureName(textureName)
	self._textureName = textureName	    
    if (not self:isCoreFacet()) and self:system():isAutoCreationEnabled() then
        self:createFacetInCore()
    else
        self:setTexture()
    end
end

function TextureFacet:setTextureCoordinates(sourceRect)
	self._textureCoordinates = sourceRect
    self:setTexture()
end

function TextureFacet:setTexture()
    if self:isCoreFacet() then 
        local textureChange = {
            typeName = "TextureChangeEvent",
            entityId = self:of():id(),            
            textureName = self:textureName(),
            sourceTextureRect = self:textureCoordinates(),
            facetId = self:id()
        }
        EventProcessor.process(textureChange)
    end
end


function TextureFacet:textureName()	
	return self._textureName
end

function TextureFacet:textureCoordinates()
	return self._textureCoordinates or {0,0,0,0}
end

function TextureFacet:offset()
	return self._offset or {0,0}
end
	
function TextureFacet:setOffset(offset)
	self._offset = offset
	if self:isCoreFacet() then	
		local offsetChange = {
			typeName = "OffsetChangeEvent",
			entityId = self:of():id(),
			offset = offset,
            facetId = self:id()
		}
		EventProcessor.process(offsetChange)	
	end
end

function TextureFacet:scaleToEntity()
    if self:isCoreFacet() then        
        local dimensions = self:of():dimensions()            
        local myDimensions = self:dimensions()        
        local scale = self:scale()        
        local newScale = {dimensions[1] * scale[1] / myDimensions[1], dimensions[2] * scale[2] / myDimensions[2] }        
        self:setScale(newScale)
    elseif self._textureCoordinates and self._textureCoordinates[3] ~= 0 and self._textureCoordinates[4] ~= 0 then        
        local dimensions = self:of():dimensions() 
        local newScale = {dimensions[1] / self._textureCoordinates[3], dimensions[2] / self._textureCoordinates[3]}
        self:setScale(newScale)
    else        
        self:setScale({1,1})
    end
       
end

function TextureFacet:setScale(scale)
	self._scale = scale
    if self:isCoreFacet() then
        local scaleChange = {
            typeName = "ScaleChangeEvent",
            entityId = self:of():id(),
            scale = self:scale(),
            facetId = self:id()
        }
        EventProcessor.process(scaleChange)           
    end
end
   

function TextureFacet:scale()
	return self._scale or {1,1}
end

function TextureFacet:color()
	return self._color or {1,1,1,1}
end


function TextureFacet:createFacetInCore()
	if not self:isCoreFacet() then
		local facet = {
			source = self:textureCoordinates(),	
			textureName = self:textureName(),
			position = self:of():position(),			
			scale = self:scale(),
			offset = self:offset()
		}		
		self._id = addTextureFacet_bind(self:system():name(), self:of():id(), facet)
				
	end
	
end
