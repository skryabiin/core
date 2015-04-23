TextFacet = Facet:new()

function TextFacet:new(entity, o)
	o = o or {}
	return self:create(entity, o)
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
	self:updateFacetInCore()
end

function TextFacet:updateFacetInCore()	
	if self:isCoreFacet() then
		local textContentChange = {
			typeName = "TextContentChange",
			entityId = self.of:getId(),			
			offset = self:getVisualOffset(),
			font = self:getFont(),
			textContent = self:getText(),
			color = self:getColor(),
			facetId = self:getId()
		}
		
		EventProcessor.process(textContentChange)
	end
end

	
function TextFacet:getVisualOffset()
	return self.offset or {0,0}
end
	
	
function TextFacet:setVisualOffset(offset)
	self.offset = offset
	self:updateFacetInCore()
end

function TextFacet:setVisualScale(scale)
	self.scale = scale
end

function TextFacet:getVisualScale()
	return self.scale or {1,1}
end

function TextFacet:getColor()
	return self.color or {255,255,255,255}
end

function TextFacet:setColor(color)
	self.color = color
	self:updateFacetInCore()
end

function TextFacet:setFont(font)
	self.font = font
	self:updateFacetInCore()
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
			position = self.of:getPosition(),
			offset = self:getVisualOffset(),
			font = self:getFont(),
			text = self:getText(),
			color = self:getColor()
		}
			
		self.id = addTextFacet_bind(self.systemName, self.of:getId(), textFacet)		
		self:setIsCoreFacet(true)
	end
end
		