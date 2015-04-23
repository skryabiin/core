mouse = {
	mousePickedUp = nil,
	mousePickedUpRelative = {0,0},
	maxSelectedZ = 0,
	maxZIndex = 0,
	selectedPosition = nil,
	mouseCursor = nil,
	mouseCursorPos = nil,
	
	init = function()
		mouse.mouseCursor = core.createEntity()

		mouse.mouseCursorPos = core.createEntity()
		mouse.mouseCursorPos:setText({50,50,0}, "(0,0)", {0, 0}, "default", {255, 255, 255, 255}, "Text")

		mouse.mouseCursorPos.draggable = true

		local cursorTexture = {
			source = {0, 0, 24, 32},
			dimensions = {24, 32},
			textureName = "cursor_brown",
			position = {0, 0, 200},	
			scale = {1, 1},
			offset = {0, -32}
		}

		core.createSystem("TextureRenderSystem", "MouseTexture", 10)

		addTextureFacet_bind("MouseTexture", mouse.mouseCursor.id, cursorTexture)

		eventProcessor.addEventFilter(0, "MouseEvent", mouse.mouseCallback)

	end,
	
	mouseCallback = function(e)

		local newPos = {e.position[1], e.position[2], 200}
		mouse.mouseCursor:setPosition(newPos)
		mouse.mouseCursorPos:setText({50, 50, 0},"(" .. e.position[1] .. "," .. e.position[2] .. ")", {0,0}, "default", {255,255,255,255}, "Text")
		if e.m1down == true and e.m1downOld == false then
		
			local entitiesInRectQuery = {
				typeName = "EntitiesInRectQuery",
				rect = {newPos[1], newPos[2], newPos[1] + 1, newPos[2] + 1},
				entities = {}
			}
			
			entitiesInRectQuery = eventProcessor.process(entitiesInRectQuery)				
			local clickedOnDraggable = false
			for i, v in ipairs(entitiesInRectQuery.entities) do
				local thisEntity = core.entities[v]
				local currentPos = thisEntity:getPosition()
				if thisEntity.draggable == true then
					clickedOnDraggable = true
					if mouse.mousePickedUp == nil then
						mouse.mousePickedUp = thisEntity
						mouse.selectedPosition = currentPos
						mouse.maxSelectedZ = currentPos[3]	
						mouse.mousePickedUpRelative = {currentPos[1] - newPos[1], currentPos[2] - newPos[2]}
					else 				
						if currentPos[3] > maxSelectedZ then
							mouse.selectedPosition = currentPos
							mouse.mousePickedUp = thisEntity
							mouse.mousePickedUpRelative = {currentPos[1] - newPos[1], currentPos[2] - newPos[2]}
							mouse.maxSelectedZ = currentPos[3]
						end
					end
				end
				console.debug("Entity " .. v .. " found.")			
			end
			if clickedOnDraggable == true then
				mouse.maxZIndex = getMaxZIndex_bind(mouse.mousePickedUp.layerId)
				mouse.selectedPosition[3] = mouse.maxZIndex + 1
				mouse.mousePickedUp:setPosition(mouse.selectedPosition)
				console.debug("Updating z index to ", mouse.selectedPosition[3])
			end
				
			
		elseif e.m1down == true and e.m1downOld == true and mouse.mousePickedUp ~= nil then
			
			local newPickedUpPos = {newPos[1] + mouse.mousePickedUpRelative[1],newPos[2] + mouse.mousePickedUpRelative[2], mouse.maxZIndex + 1}
			mouse.mousePickedUp:setPosition(newPickedUpPos)
		elseif e.m1down == false then
			mouse.mousePickedUp = nil
			mouse.mousePickedUpRelative = {0,0}
			mouse.selectedPosition = nil			
			mouse.maxZIndex = 0
			clickedOnDraggable = false
		end
			
	end


}