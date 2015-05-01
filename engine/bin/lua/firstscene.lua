
Scenes.first = {}
function Scenes.first.init()
	--set this up for the sequence loop

	Console.info("In first scene")

	Core.createSystem("BasicPositionSystem2d", "MenuPositions")
	Core.createSystem("TextureRenderSystem2d", "MenuTextures", 1)
	Core.createSystem("TextRenderSystem2d", "MenuText", 1)
		
	currentPositionSystemPtr = Core.createSystem("BasicPositionSystem2d", "Positions")
	currentTextureRenderSystemPtr = Core.createSystem("TextureRenderSystem2d", "Textures", 2)
	currentTextRenderSystemPtr = Core.createSystem("TextRenderSystem2d", "Text", 2)	
    local cursorTexture = {
			source = {0, 0, 24, 32},
			dimensions = {24, 32},
			textureName = "cursor_brown",			
			scale = {1, 1},
			offset = {0, -32},			
			textureColorMod = {100,255,255,100}
		}
        
Interface.setCursorTexture(cursorTexture)

	TestEntity = {}

	function TestEntity:new(testEntity)
	testEntity = testEntity or {}
	testEntity = Entity:new(testEntity)
	testEntity:addBasicPosition("Positions")
	testEntity:setDimensions({64,64})		
	testEntity:setPosition({300,300,0})
	testEntity.facets.texture = TextureFacet:new(testEntity)
	testEntity.facets.texture:setSystemName("Textures")
	testEntity.facets.texture:setTextureName("redBox")
	testEntity.facets.texture:setTextureCoordinates({0,0,32,32})
	--testEntity.facets.texture:setScale({2,1})
	testEntity.facets.texture:setOffset({0,0})
	testEntity.facets.texture:createFacetInCore()
    testEntity.facets.texture:scaleToEntity()
	testEntity.facets.interface:setDraggable(true)
	testEntity.facets.interface:setHoverable(false)
	testEntity.facets.interface:setClickable(true)

	return testEntity
	end


	testEntity1 = TestEntity:new()

	actionMenu = Menu:new()

	actionMenu:setPosition({500,300,0})
	actionMenu.closeBox:align()


	urologyMenu = Entity:new()
	urologyMenu:addBasicPosition("Positions")
	urologyMenu:setDimensions({40,21})
	urologyMenu.facets.label = TextFacet:new(urologyMenu)
	urologyMenu.facets.label:setText("Urology")
	urologyMenu.facets.label:setSystemName("Text")
	urologyMenu.facets.label:createFacetInCore()
	urologyMenu.facets.interface:setDraggable(false)
	urologyMenu.facets.interface:setClickable(true)
	urologyMenu.facets.interface:setOnClick(
	function(self, interfaceState)
		actionMenu:resume()
	end
	)
	urologyMenu:setPosition({10, Config.window[2] - 30,10})

	quitButton = Entity:new()
	quitButton:addBasicPosition("Positions")
	quitButton:setDimensions({40,21})
	quitButton.facets.label = TextFacet:new(quitButton)
	quitButton.facets.label:setText("Quit")
	quitButton.facets.label:setSystemName("Text")
	quitButton.facets.label:createFacetInCore()
	quitButton.facets.interface:setDraggable(false)
	quitButton.facets.interface:setClickable(true)
	quitButton.facets.interface:setOnClick(
	function(self, interfaceState)
		Core.setNextScene(Scenes.title)
		Core.quit("Transitioning to title scene")
	end
	)
	quitButton:setPosition({60, Config.window[2] - 30,10})





	actionMenu.facets.interface:setOnDrag(Interface.functions.basicDragMove)
	testEntity1.facets.interface:setOnDrag(Interface.functions.basicDragMove)
	actionMenu:addChild(testEntity1)
	--actionMenu:pause()
end

function Scenes.first.update(dt) 
	
end

function Scenes.first.reset()

end