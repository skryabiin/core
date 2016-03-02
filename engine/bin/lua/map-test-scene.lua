Scenes.maptest = {}

function Scenes.maptest.keyboardCallback(keyboardEvent)

		if keyboardEvent.keysym == sdl.key.w then                
           Scenes.maptest.moveup = keyboardEvent.down
		end
		if keyboardEvent.keysym == sdl.key.a then
                Scenes.maptest.moveleft = keyboardEvent.down
		end
        if keyboardEvent.keysym == sdl.key.s then
                Scenes.maptest.movedown = keyboardEvent.down
		end
        if keyboardEvent.keysym == sdl.key.d then
                Scenes.maptest.moveright = keyboardEvent.down
		end
end

function Scenes.maptest.tileParseCallback(tileInformation)
    local e = Entity:new()    
    e:setPosition(tileInformation.position)
    e:setDimensions(tileInformation.dimensions)
    
    e.facets.texture = TextureFacet:new(e, Scenes.maptest.textures, tileInformation.textureName)
    e.facets.texture:setTextureCoordinates({tileInformation.sourceTexturePosition[1], tileInformation.sourceTexturePosition[2], tileInformation.dimensions[1], tileInformation.dimensions[2]})    
    --e.facets.texture:setScale({2.0,2.0})
    e.facets.texture:createFacetInCore()
    

end

function Scenes.maptest.init()

    Scenes.maptest.keyboardCallbackRef = EventProcessor.addEventListener(-1,"KeyboardEvent", Scenes.maptest.keyboardCallback)
        
    Scenes.maptest.textures = TextureRenderSystem2d:new("MapTextures",1)    
    Scenes.maptest.textures:disableAutoCreation()    

    World.camera.centerAt({0,0,0})
    World.runMap("Level1", Scenes.maptest.tileParseCallback)

    Scenes.maptest.moveleft = false
    Scenes.maptest.moveright = false
    Scenes.maptest.moveup = false
    Scenes.maptest.movedown = false
    Scenes.maptest.pps = 300

end

function Scenes.maptest.update(dt)

    local moveCamera = {0,0,0}

    if Scenes.maptest.moveleft then
        moveCamera[1] = moveCamera[1] - dt * Scenes.maptest.pps / 1000.0
    end
    if Scenes.maptest.moveright then
        moveCamera[1] = moveCamera[1] + dt * Scenes.maptest.pps / 1000.0
    end
    if Scenes.maptest.moveup then
        moveCamera[2] = moveCamera[2] + dt * Scenes.maptest.pps / 1000.0        
    end
    if Scenes.maptest.movedown then
        moveCamera[2] = moveCamera[2] - dt * Scenes.maptest.pps / 1000.0
    end
    
    if moveCamera[1] ~= 0 or moveCamera[2] ~= 0 then        
        World.camera.move(moveCamera)
    end

end

function Scenes.maptest.reset()

end