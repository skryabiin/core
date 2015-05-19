TextFacet = Facet:new()

function TextFacet:new(entity, system, o)
	o = o or {}
	return self:create(entity, system, o)
end

function TextFacet:type()
	return "TextFacet"
end

function TextFacet:isRenderableFacet()
	return true
end

function TextFacet:text()
	return self._text or ""
end

function TextFacet:setText(text)
	self._text = text
    if (not self:isCoreFacet()) and self:system() then
        self:createFacetInCore()
    elseif self:isCoreFacet() then	
		local textContentChange = {
			typeName = "TextContentChangeEvent",
			entityId = self:of():id(),			
			font = self:font(),
			textContent = self:text(),
			facetId = self:id()
		}		
		EventProcessor.process(textContentChange)
	end
end
	
function TextFacet:offset()
	return self._offset or {0,0}
end
	
	
function TextFacet:setOffset(offset)
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

function TextFacet:scaleToEntity()
    if self:isCoreFacet() then
        local dimensions = self:of():dimensions()
        local myDimensions = self:dimensions()
        local scale = self:scale()
        local newScale = {dimensions[1] * scale[1] / myDimensions[1], dimensions[2] * scale[2] / myDimensions[2] }
        self:setScale(scale)
    else 
        self:setScale({1,1})
    end
end

function TextFacet:setScale(scale)
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

function TextFacet:scale()
	return self._scale or {1,1}
end

function TextFacet:color()
	return self._color or {1,1,1,1}
end

function TextFacet:setColor(color)
	self._color = color
    if self:isCoreFacet() then	
		local colorChange = {
			typeName = "ColorChangeEvent",
			entityId = self:of():id(),            
            color = self:color(),
            facetId = self:id()
		}
		EventProcessor.process(colorChange)	
	end
end

function TextFacet:setFont(font)
	self._font = font
	self:setText(self._text)
end

function TextFacet:font()
	if self._font then		
		return self._font
	else
		local defaultFont = Fonts.defaultFont()
		return defaultFont.name
	end
end

function TextFacet:createFacetInCore()
	if not self:isCoreFacet() then
		local textFacet = {
			position = self:of():position(),
			offset = self:offset(),
            scale = self:scale(),
			font = self:font(),
			text = self:text(),
			color = self:color()
		}
			
		self._id = addTextFacet_bind(self:system():name(), self:of():id(), textFacet)				
	end
end
		