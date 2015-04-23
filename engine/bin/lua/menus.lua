



Menu = {
	functions = {}
}



function Menu:new(menu)
	menu = menu or {}
	menu = Entity:new(menu)
	menu:addBasicPosition("MenuPositions")	
	menu:setDimensions({200,400})
	menu.facets.background = TextureFacet:new(menu)
	menu.facets.background:setSystemName("MenuTextures")
	menu.facets.background:setTextureName("blueBalls")
	menu.facets.background:createFacetInCore()
	menu.facets.interface:setDraggable(true)
	menu.facets.interface:setOnDrag(Interface.functions.basicDragMove)
	menu.facets.interface:setHoverable(false)
	menu.facets.interface:setClickable(true)
	menu.facets.interface:setOnClick(function() 
									end)
	menu.facets.title = TextFacet:new(menu)
	menu.facets.title:setSystemName("MenuText")
	menu.facets.title:setText("Menu " .. menu:getId())
	menu.facets.title:setVisualOffset({0,0,1})
	menu.facets.title:createFacetInCore()
	
	function menu:align()
		self.closeBox:align()
	end
		
	
	local closeBox = Entity:new()
	menu:addChild(closeBox)
	menu.closeBox = closeBox
	closeBox:addBasicPosition("MenuPositions")
	closeBox:setDimensions({16,16})
	closeBox.facets.texture = TextureFacet:new(closeBox)
	closeBox.facets.texture:setSystemName("MenuTextures")
	closeBox.facets.texture:setTextureName("redBox")
	closeBox.facets.texture:createFacetInCore()
	closeBox.facets.interface:setDraggable(false)
	closeBox.facets.interface:setClickable(true)
	closeBox.facets.interface:setOnClick(
		function(self, interfaceState)
			local parent = self:getOf():getParent()
			parent:pause()

		end
	)
	closeBox.facets.interface:setOnDrag(
		function(self, interfaceState)			
			self:getOf():align()
		end
	)
	function closeBox:align()		
		local position = self:getParent():getPosition()
		local dimensions = self:getParent():getDimensions()
		local newPos = {}
		newPos[1] = position[1] + dimensions[1] - 18
		newPos[2] = position[2] + dimensions[2] - 18
		newPos[3] = position[3] + 1
		self:setPosition(newPos)
	end
	
	
	
	return menu
end
	
	
