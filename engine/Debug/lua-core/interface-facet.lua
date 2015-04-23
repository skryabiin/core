

InterfaceFacet = Facet:new()

function InterfaceFacet:new(entity, o)
	o = o or {}
	return self:create(entity, o)
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
	return self.draggable
end

function InterfaceFacet:isClickable()
	return self.clickable
end

function InterfaceFacet:isHoverable()
	return self.hoverable
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

function InterfaceFacet:getOnClick()
	if not self.onClick then
		self:setOnClick(Interface.functions.basicOnClick)
	end
	return self.onClick
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
		self:setOnDrag(Interface.functions.basicDragMove)
	end
	return self.onDrag
end

function InterfaceFacet:updateFacetInCore()
	local facetDef = { draggable = self:isDraggable(),
							clickable = self:isClickable(),
							hoverable = self:isHoverable(),
							onClick = self:getOnClick(),
							onDrag = self:getOnDrag()}
	self.id = updateInterfaceFacet_bind(self.of:getId(), facetDef)
end