

InterfaceFacet = Facet:new()

function InterfaceFacet:new(entity, o)
	o = o or {}
	return self:create(entity, "Interface", o)
end

function InterfaceFacet:getType()
	return "InterfaceFacet"
end

function InterfaceFacet:getSystemName()
	return "Interface"
end

function InterfaceFacet:setSystemName(systemName)
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
	self.onClick(interfaceState)
end

function InterfaceFacet:setOnClick(onClick)	
		local myself = self
		self.onClick = function(interfaceState)			
			onClick(myself, interfaceState)
		end
		self:updateFacetInCore()	
end

function InterfaceFacet:setOffClick(offClick)
        local myself = self 
        self.offClick = function(interfaceState)
            offClick(myself, interfaceState)
        end
        self:updateFacetInCore()
end

function InterfaceFacet:getOnClick()
	if not self.onClick then
        local myself = self
        self.onClick = function(interfaceState)
            Interface.functions.basicOnClick(myself, interfaceState)
        end 		
	end
	return self.onClick
end

function InterfaceFacet:getOffClick()
    if not self.offClick then
        local myself = self
        self.offClick = function(interfaceState)
            Interface.functions.basicOffClick(myself, interfaceState)
        end 
    end
    return self.offClick
end


function InterfaceFacet:setDraggable(draggable)
	if self.draggable ~= draggable then
		self.draggable = draggable
		self:updateFacetInCore()
	end	
end

function InterfaceFacet:setOnDrag(onDrag)
	local myself = self
	self.onDrag = function(interfaceState)
		onDrag(myself, interfaceState)
	end	
	self:updateFacetInCore()

end

function InterfaceFacet:getOnDrag(onDrag)
	if not self.onDrag then
        local myself = self
        self.onDrag = function(interfaceState)
            Interface.functions.basicOnDrag(myself, interfaceState)
        end 		
	end
	return self.onDrag
end

function InterfaceFacet:setOnHover(onHover)
	local myself = self
	self.onHover = function(interfaceState)
		onHover(myself, interfaceState)
	end	
	self:updateFacetInCore()
end

function InterfaceFacet:setOffHover(offHover)
	local myself = self
	self.offHover = function(interfaceState)
		offHover(myself, interfaceState)
	end	    
	self:updateFacetInCore()
end


function InterfaceFacet:getOnHover()
    if not self.onHover then
        local myself = self
        self.onHover = function(interfaceState)
            Interface.functions.basicOnHover(myself, interfaceState)
        end 
    end
    return self.onHover
end

function InterfaceFacet:getOffHover()
    if not self.offHover then
        local myself = self
        self.offHover = function(interfaceState)
            Interface.functions.basicOffHover(myself, interfaceState)
        end        
    end
    return self.offHover
end

function InterfaceFacet:createFacetInCore()
   local facetDef = {   position = self:getOf():getPosition(),
                        dimensions = self:getOf():getDimensions(),
                        draggable = self:isDraggable(),
					    clickable = self:isClickable(),
					    hoverable = self:isHoverable(),
					    onClick = self:getOnClick(),
                        offClick = self:getOffClick(),
                        onHover = self:getOnHover(),
                        offHover = self:getOffHover(),	
    				    onDrag = self:getOnDrag()}
    self.id = createInterfaceFacet_bind(self.of:getId(), facetDef)    
end

function InterfaceFacet:updateFacetInCore()
    if self:getId() ~= -1 then        
	    local facetDef = {  draggable = self:isDraggable(),
					        clickable = self:isClickable(),
					        hoverable = self:isHoverable(),
					        onClick = self:getOnClick(),
                            offClick = self:getOffClick(),
                            onHover = self:getOnHover(),
                            offHover = self:getOffHover(),
					        onDrag = self:getOnDrag()}
	    updateInterfaceFacet_bind(self:getId(), facetDef)
    else         
        self:createFacetInCore()
    end
end