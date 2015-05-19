

InterfaceFacet = Facet:new()

function InterfaceFacet:new(entity, o)
	o = o or {}
	return self:create(entity, Interface, o)
end

function InterfaceFacet:type()
	return "InterfaceFacet"
end

function InterfaceFacet:setSystem(system)
	--do nothing
end

function InterfaceFacet:isDraggable()
	return self.draggable or false
end

function InterfaceFacet:isClickable()
	return self.clickable or false
end

function InterfaceFacet:isHoverable()
	return self.hoverable or false
end

function InterfaceFacet:setHoverable(hoverable)
	if self.hoverable ~= hoverable then
		self.hoverable = hoverable
		self:updateFacetInCore()
	end	
end

function InterfaceFacet:setClickable(clickable)	
	if self.clickable ~= clickable then
		self.clickable = clickable
		self:updateFacetInCore()	
	end
end

function InterfaceFacet:doOnClick(interfaceState)
	self._onClick(interfaceState)
end

function InterfaceFacet:setOnClick(onClick)	
		local myself = self
		self._onClick = function(interfaceState)			
			onClick(myself, interfaceState)
		end
		self:updateFacetInCore()	
end

function InterfaceFacet:setOffClick(offClick)
        local myself = self 
        self._offClick = function(interfaceState)
            offClick(myself, interfaceState)
        end
        self:updateFacetInCore()
end

function InterfaceFacet:onClick()
	if not self._onClick then
        local myself = self
        self._onClick = function(interfaceState)
            Interface.functions.basicOnClick(myself, interfaceState)
        end 		
	end
	return self._onClick
end

function InterfaceFacet:offClick()
    if not self._offClick then
        local myself = self
        self._offClick = function(interfaceState)
            Interface.functions.basicOffClick(myself, interfaceState)
        end 
    end
    return self._offClick
end


function InterfaceFacet:setDraggable(draggable)
	if self.draggable ~= draggable then
		self.draggable = draggable
		self:updateFacetInCore()
	end	
end

function InterfaceFacet:setOnDrag(onDrag)
	local myself = self
	self._onDrag = function(interfaceState)
		onDrag(myself, interfaceState)
	end	
	self:updateFacetInCore()

end

function InterfaceFacet:onDrag(onDrag)
	if not self._onDrag then
        local myself = self
        self._onDrag = function(interfaceState)
            Interface.functions.basicOnDrag(myself, interfaceState)
        end 		
	end
	return self._onDrag
end

function InterfaceFacet:setOnHover(onHover)
	local myself = self
	self._onHover = function(interfaceState)
		onHover(myself, interfaceState)
	end	
	self:updateFacetInCore()
end

function InterfaceFacet:setOffHover(offHover)
	local myself = self
	self._offHover = function(interfaceState)
		offHover(myself, interfaceState)
	end	    
	self:updateFacetInCore()
end


function InterfaceFacet:onHover()
    if not self._onHover then
        local myself = self
        self._onHover = function(interfaceState)
            Interface.functions.basicOnHover(myself, interfaceState)
        end 
    end
    return self._onHover
end

function InterfaceFacet:offHover()
    if not self._offHover then
        local myself = self
        self._offHover = function(interfaceState)
            Interface.functions.basicOffHover(myself, interfaceState)
        end        
    end
    return self._offHover
end

function InterfaceFacet:createFacetInCore()
   local facetDef = {   position = self:of():position(),
                        dimensions = self:of():dimensions(),
                        draggable = self:isDraggable(),
					    clickable = self:isClickable(),
					    hoverable = self:isHoverable(),
					    onClick = self:onClick(),
                        offClick = self:offClick(),
                        onHover = self:onHover(),
                        offHover = self:offHover(),	
    				    onDrag = self:onDrag()}
    self._id = createInterfaceFacet_bind(self:of():id(), facetDef)    
end

function InterfaceFacet:updateFacetInCore()
    if self:id() ~= -1 then        
	    local facetDef = {  draggable = self:isDraggable(),
					        clickable = self:isClickable(),
					        hoverable = self:isHoverable(),
					        onClick = self:onClick(),
                            offClick = self:offClick(),
                            onHover = self:onHover(),
                            offHover = self:offHover(),
					        onDrag = self:onDrag()}
	    updateInterfaceFacet_bind(self:id(), facetDef)
    else         
        self:createFacetInCore()
    end
end