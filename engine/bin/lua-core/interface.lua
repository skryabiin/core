
Interface = {
	functions = {}
}



function Interface:type()
    return "Interface"
end

function Interface:setName(systemName)
    --do nothing
end

function Interface:name()
    return "Interface"
end

function Interface.setCursorTexture(cursorTexture)
	setCursorTexture_bind(cursorTexture)
end
function Interface.keyStates()
    return getKeyStates_bind()
end

function Interface.gamepadStates()
    return getGamepadStates_bind()
end

function Interface.showCursor()
    showCursor_bind()
end

function Interface.hideCursor()
    hideCursor_bind()
end

function Interface.useSystemCursor()
    useSystemCursor_bind()
end

function Interface.useTextureCursor()
    useTextureCursor_bind()
end

function Interface.functions.basicOnHover(self, interfaceState)
    Console.debug("Hovering on entity " , self:of():id() , ", facet ", self:id())
end

function Interface.functions.basicOffHover(self, interfaceState)
    Console.debug("Done hovering on entity ", self:of():id(), ", facet ", self:id())
end

function Interface.functions.basicOnClick(self, interfaceState)
	Console.debug("clicked on entity " , self:of():id() , ", facet " , self:id())
end

function Interface.functions.basicOffClick(self, interfaceState)
    Console.debug("click released on entity " ,self:of():id() , ", facet " , self:id())
end

function Interface.functions.basicDragMove(self, interfaceState)
	if self:of():isPaused() then 
		return
	end
	
	if interfaceState.pickedUpThisTick then
		local position = self:of():position()
		self.dragDelta = Util.subtractPositions(position,interfaceState.clickPosition)		
		local dimensions = self:of():dimensions()		
		self.dragDelta[3] = position[3]
		self.dragDelta[4] = dimensions[1]
		self.dragDelta[5] = dimensions[2]
	else 				
		local newPosition = Util.addPositions(interfaceState.currentPosition, self.dragDelta)			
		newPosition[3] = self.dragDelta[3]		
		newPosition[1] = Util.bounds(newPosition[1], 0, Config.window[1] - self.dragDelta[4])
		newPosition[2] = Util.bounds(newPosition[2], 0, Config.window[2] - self.dragDelta[5])
		self:of():setPosition(newPosition)
	end
	
	for i, v in pairs(self:of():children()) do	
		local childOnDrag = v.facets.interface:getOnDrag()
		childOnDrag(interfaceState)
	end
	
end


function Interface.functions.gridDragMove(self, interfaceState)		
	if interfaceState.pickedUpThisTick then
		self.dragDelta = Util.subtractPositions(interfaceState.pickedUpPosition,interfaceState.clickPosition)
		self.dragDelta[3] = interfaceState.pickedUpPosition[3] --just want to keep the current z so don't need too many "get position" calls
		local dimensions = self:of():dimensions()
		self.dragDelta[4] = dimensions[1]
		self.dragDelta[5] = dimensions[2]
	else 				
		local newPosition = Util.addPositions(interfaceState.currentPosition, self.dragDelta)
			newPosition[1] = newPosition[1] - newPosition[1] % 50
			newPosition[2] = newPosition[2] - newPosition[2] % 50
			newPosition[1] = Util.bounds(newPosition[1], 0, Config.window[1] - self.dragDelta[4])
			newPosition[2] = Util.bounds(newPosition[2], 0, Config.window[2] - self.dragDelta[5])
			newPosition[3] = self.dragDelta[3]
		self:getOf():setPosition(newPosition)
	end
end


function Interface.functions.alphaFun(self, interfaceState)	

	if interfaceState.pickedUpThisTick then		
		self.alphaBase = self.newAlpha or 255
	else
		self.newAlpha = self.alphaBase - (interfaceState.clickPosition[2] - interfaceState.currentPosition[2])
		self.newAlpha = Util.bounds(self.newAlpha, 0, 255)
		self:of().facets.background:setColor({1,1,1,self.newAlpha})	
	end
end



function Interface.dragComposite(self, interfaceState)
	Interface.functions.alphaFun(self, interfaceState)
	Interface.functions.basicDragMove(self, interfaceState)
end