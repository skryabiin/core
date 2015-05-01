
Scenes.title = {}

function Scenes.title.keyboardCallback(keyboardEvent)

	Console.info("Key is " , keyboardEvent.down)

	if keyboardEvent.down then
		Console.info("Key pressed is ",keyboardEvent.keysym)
			local selectedIndex = 1
			for i, v in pairs(titleButtons) do
				if v.selected then
					selectedIndex = i
				end
			end		
	
		if keyboardEvent.keysym == sdl.key.space then			
			titleButtons[selectedIndex]:doUnselect()
			
			if selectedIndex == #titleButtons then
				selectedIndex = 1
			else
				selectedIndex = selectedIndex + 1
			end
			
			titleButtons[selectedIndex]:doSelect()
		elseif keyboardEvent.keysym == sdl.key.returnkey then			
			titleButtons[selectedIndex].facets.interface:doOnClick()			
		end
		
	end
end

function Scenes.title.init()


	Console.info("In title scene")

	Core.createSystem("BasicPositionSystem2d", "MenuPositions")
	Core.createSystem("TextureRenderSystem2d", "MenuTextures", 1)
	Core.createSystem("TextRenderSystem2d", "MenuText", 1)
	
	Scenes.title.keyboardCallbackRef = EventProcessor.addEventFilter(-1,"KeyboardEvent", Scenes.title.keyboardCallback)
	
	TitleButton = {}
	
	function TitleButton:new(o)
		o = o or {}
		o = Entity:new(o)
		o:addBasicPosition("MenuPositions")
		o.facets.label = TextFacet:new(o)
		o.facets.label:setSystemName("MenuText")
		o.facets.interface:setDraggable(false)
		o.facets.interface:setClickable(true)
		o.selected = false
		function o:setBaseText(baseLabelText)
			o.baseLabelText = baseLabelText
			o.facets.label:setText(baseLabelText)
		end
		function o:setCenteredPosition(y)	
			local dimensions = self.facets.label:getDimensions()	
			local x = (Config.window[1] - dimensions[1])/ 2
			self:setPosition({x, y, 10})
			self:setDimensions(dimensions)
		end
		function o:doSelect()		
			if not o.selected then			
				o.facets.label:setText("+ " .. o.baseLabelText .. " +")
				local position = o:getPosition()
				o:setCenteredPosition(position[2])
				o.selected = true			
			end
		end
		
		function o:doUnselect()			
			if o.selected then								
				o.facets.label:setText(o.baseLabelText)			
				local position = o:getPosition()				
				o:setCenteredPosition(position[2])				
				o.selected = false				
			end
		end
		return o
	end
	
	titleButtons = {}
	
	
	
	playButton = TitleButton:new()
	

		
	playButton:setBaseText("Play")
	playButton:doSelect()
	playButton.facets.label:createFacetInCore()
	playButton.facets.interface:setOnClick(
		function(self, interfaceState)	
			Core.setNextScene(Scenes.first)
			Core.quit("Transitioning to first scene")
		end
	)
	playButton:setCenteredPosition(500)
		titleButtons[1] = playButton
	
	quitButton = TitleButton:new()	
	quitButton:setBaseText("Quit")	
	quitButton.facets.label:createFacetInCore()
	quitButton.facets.interface:setOnClick(
		function(self, interfaceState)
			Core.quit("Quit")
		end
	)
	
	
	
	quitButton:setCenteredPosition(400)
	
	titleButtons[2] = quitButton
	
	arborTitle = TitleButton:new()
	arborTitle.facets.label:setText("A r b o r")
	arborTitle.facets.label:createFacetInCore()
	arborTitle.facets.interface:setDraggable(false)
	arborTitle.facets.interface:setClickable(false)
	arborTitle:setCenteredPosition(600)
	
	
	
end

function Scenes.title.update(dt)

end


function Scenes.title.reset() 

	--EventProcessor.removeEventFilter(Scenes.title.keyboardCallbackRef)

end