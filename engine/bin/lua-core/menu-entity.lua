
MenuEntity = Entity:new()

function MenuEntity:new(o)
	o = self:create(o)
end

function MenuEntity:setName(name)
	self.name = name
end

function MenuEntity:setBackgroundTextureName(textureName)
	self.backgroundTextureName = textureName
end

function MenuEntity:setBackgroundColorMod(backgroundColorMod)
	self.backgroundColorMod = backgroundColorMod
end


Slider = Entity:new()

function Slider:new(vertical, default, o)
    o = o or {}
    o = self:create(o)
    o:setDefault(default)
    o:setVertical(vertical)
    o.facets.interface = InterfaceFacet:new(o)
    o.facets.interface:setDraggable(true)
    o.facets.interface:setOnDrag(
        function(self, interfaceState) 
            local cursorPosition
            if interfaceState.pickedUpThisTick then
                cursorPosition = interfaceState.clickPosition
            else
                cursorPosition = interfaceState.currentPosition
            end
            local position = self:getOf():getPosition()
            local dimensions = self:getOf():getDimensions()
            local facetDimensions = self:getOf().facets.texture:getDimensions()            
            if self:getOf():isVertical() then
                    self:getOf():setValue((cursorPosition[2] - position[2] - facetDimensions[2] / 2.0) / (dimensions[2] - facetDimensions[2]), false, dimensions, facetDimensions)
            else
                    self:getOf():setValue((cursorPosition[1] - position[1] - facetDimensions[1] / 2.0) / (dimensions[1] - facetDimensions[1]), false, dimensions, facetDimensions)
            end
	    end
    )
    return o
end

function Slider:setDefault(default)
    self.default = default
end

function Slider:getDefault()
    return self.default or 0
end
function Slider:setVertical(vertical)
    self.vertical = vertical
end

function Slider:isVertical()
    return self.vertical or false
end

function Slider:setOnChange(onChange)
    self.onChange = onChange
end

function Slider:getValue()
    return self.value or 0
end

function Slider:reset()    
        self:setValue(self:getDefault())
end    

function Slider:setValue(value, positionAlreadyUpdated, dimensions, facetDimensions)
    value = Util.bounds(value, 0, 1.0)
    self.value = value
    if not positionAlreadyUpdated then
        dimensions = dimensions or self:getDimensions()                
        local offset = self.facets.texture:getOffset()
        local facetDimensions = facetDimensions or self.facets.texture:getDimensions()        
        if self:isVertical() then 
            self.facets.texture:setOffset({offset[1], value * (dimensions[2] - facetDimensions[2])})
        else
            self.facets.texture:setOffset({value * (dimensions[1] - facetDimensions[1]), offset[2]})
        end
    end
    self.onChange(self, value)
end

