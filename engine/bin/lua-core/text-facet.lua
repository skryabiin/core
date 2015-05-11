TextFacet = Facet:new()

function TextFacet:new(entity, systemName, o)
	o = o or {}
	return self:create(entity, systemName, o)
end

function TextFacet:getType()
	return "TextFacet"
end

function TextFacet:isRenderableFacet()
	return true
end

function TextFacet:getText()
	return self.text or ""
end

function TextFacet:setText(text)
	self.text = text
    if (not self:isCoreFacet()) and self:getSystemName() then
        self:createFacetInCore()
    elseif self:isCoreFacet() then	
		local textContentChange = {
			typeName = "TextContentChangeEvent",
			entityId = self.of:getId(),			
			font = self:getFont(),
			textContent = self:getText(),
			facetId = self:getId()
		}		
		EventProcessor.process(textContentChange)
	end
end
	
function TextFacet:getOffset()
	return self.offset or {0,0}
end
	
	
function TextFacet:setOffset(offset)
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

function TextFacet:scaleToEntity()
    if self:isCoreFacet() then
        local dimensions = self.of:getDimensions()
        local myDimensions = self:getDimensions()
        local scale = self:getScale()
        local newScale = {dimensions[1] * scale[1] / myDimensions[1], dimensions[2] * scale[2] / myDimensions[2] }
        self:setScale(scale)
    else 
        self:setScale({1,1})
    end
end

function TextFacet:setScale(scale)
	self.scale = scale
    if self:isCoreFacet() then
        local scaleChange = {
            typeName = "ScaleChangeEvent",
            entityId = self.offset:getId(),
            scale = self:getScale(),
            facetId = self:getId()
        }
        EventProcessor.process(scaleChange)
    end
end

function TextFacet:getScale()
	return self.scale or {1,1}
end

function TextFacet:getColor()
	return self.color or {255,255,255,255}
end

function TextFacet:setColor(color)
	self.color = color
    if self:isCoreFacet() then	
		local colorChange = {
			typeName = "ColorChangeEvent",
			entityId = self.of:getId(),            
            color = self:getColor(),
            facetId = self:getId()
		}
		EventProcessor.process(colorChange)	
	end
end

function TextFacet:setFont(font)
	self.font = font
	self:setText(self.text)
end

function TextFacet:getFont()
	if self.font then		
		return self.font
	else
		local defaultFont = Fonts.getDefaultFont()
		return defaultFont.name
	end
end

function TextFacet:createFacetInCore()
	if not self:isCoreFacet() then
		local textFacet = {
			position = self:getOf():getPosition(),
			offset = self:getOffset(),
            scale = self:getScale(),
			font = self:getFont(),
			text = self:getText(),
			color = self:getColor()
		}
			
		self.id = addTextFacet_bind(self.systemName, self.of:getId(), textFacet)				
	end
end
		