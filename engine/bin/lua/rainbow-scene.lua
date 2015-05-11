Scenes.rainbow = {}

function Scenes.rainbow.init()

    Console.info("In rainbow scene init.")

    local rainbowColorMatrix = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}

    --Renderer.setGlobalColorModulation({0,1,0,0, 0,0,1,0, 1,0,0,0, 0,0,0,1})

    Renderer.setBackgroundColor({0,64,0,255})


	Core.createSystem("BasicPositionSystem2d", "Positions")
	Core.createSystem("TextureRenderSystem2d", "Textures", 2)
    Core.createSystem("TextRenderSystem2d", "Text", 2)
    Core.createSystem("BasicAudioSystem", "Sounds")    

	rainbow = Entity:new()
	rainbow:addBasicPosition("Positions")

    
	rainbow.facets.texture = TextureFacet:new(rainbow, "Textures", "rainbow")
	rainbow:setDimensions({640,360})		
	rainbow:setPosition({192,400,0})
    rainbow.facets.texture:scaleToEntity()
    rainbow.facets.interface = InterfaceFacet:new(rainbow)
    rainbow.facets.interface:setClickable(true)
    rainbow.facets.interface:setOffClick(
        function(self, interfaceState)
            Console.debug("In off click")
        end
    ) 

    sliders = {}
    local index = 1
    for s = 1, 12, 1 do        
        if s % 4 ~= 0 then
            local defaultSliderValue
            if s == 1 or s == 6 or s == 11 then
                defaultSliderValue = 0.5
            else
                defaultSliderValue = 0
            end
            local slider = Slider:new(true, defaultSliderValue)
            slider:addBasicPosition("Positions")            
            slider:setPosition({(Config.window.dimensions[1] / 10) * index - 8,100})
            slider:setDimensions({16,200})            
            slider.facets.clicksound = AudioFacet:new(slider, "Sounds", "DoorOpen",0.5)
            slider.facets.texture = TextureFacet:new(slider, "Textures", "redBox")            
            slider.facets.texture:setScale({0.5, 0.5})
            slider.facets.interface:setClickable(true)
            slider.facets.interface:setOnClick(
                function(self, interfaceState)                    
                        self:getOf().facets.clicksound:playSound()                    
                end
            )
            slider.facets.interface:setHoverable(true)
            slider.facets.interface:setOnHover(
                function(self, interfaceState)
                    local colorModEvent = {
                        typeName = "ColorModulationEvent",
                        entityId = self:getOf():getId(),
                        facetId = self:getOf().facets.texture:getId(),
                        matrix = {1,0,0,0, 0,1,0,0, 0.5,0,1,0, 0,0,0,1}
                    }
                    EventProcessor.process(colorModEvent)
                end
            )
            slider.facets.interface:setOffHover(
                function(self, interfaceState)
                    local colorModEvent = {
                        typeName = "ColorModulationEvent",
                        entityId = self:getOf():getId(),
                        facetId = self:getOf().facets.texture:getId(),
                        matrix = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}
                    }
                    EventProcessor.process(colorModEvent)
                end
            )
            slider:setOnChange(function(self, value)
                rainbowColorMatrix[s] = value * 2
                 local colorModEvent = {
                    typeName = "ColorModulationEvent",
                    entityId = rainbow:getId(),
                    facetId = -1,
                    matrix = rainbowColorMatrix
                }
                EventProcessor.process(colorModEvent)
            end
            )

            slider:setValue(defaultSliderValue)
            sliders[index] = slider
            index = index + 1
        end
    end    

    local volumeSlider = Slider:new(true, 0.5)
            volumeSlider:addBasicPosition("Positions")            
            volumeSlider:setPosition({(Config.window.dimensions[1] - 76),(Config.window.dimensions[2] - 130)})
            volumeSlider:setDimensions({16,100})
            volumeSlider.facets.clicksound = AudioFacet:new(volumeSlider, "Sounds", "DoorOpen",0.5)
            volumeSlider.facets.texture = TextureFacet:new(volumeSlider, "Textures", "redBox")
            volumeSlider.facets.texture:setScale({0.5, 0.5})            
            volumeSlider.facets.interface:setClickable(true)
            volumeSlider.facets.interface:setOffClick(
                function(self, interfaceState)                    
                        self:getOf().facets.clicksound:playSound()                            
                end
            )
            volumeSlider:setOnChange(function(self, value)
                local volume = value * 2
                Audio.setGlobalVolumeLevel(volume)
                
            end
            )
            volumeSlider:setValue(0.5)            

    for s=4, 9, 1 do
        local sliderColor
        if s < 7 then 
            sliderColor = {0,0,0,0, 1,0,0,0, 0,0,0,0, 0,0,0,1}
        else
            sliderColor = {0,0,0,0, 0,0,0,0, 1,0,0,0, 0,0,0,1}
        end
        local colorModEvent = {
            typeName = "ColorModulationEvent",
            entityId = sliders[s]:getId(),
            facetId = sliders[s].facets.texture:getId(),
            matrix = sliderColor
        }
        EventProcessor.process(colorModEvent)
    end

    local function fadeCallback()
        Console.debug("In fade callback!")
    end

    local function fadeCallback2()
        Console.debug("In fade callback2!")
    end
    resetButton = Entity:new()
	resetButton:addBasicPosition("Positions")
	resetButton:setDimensions({50,21})
	resetButton.facets.label = TextFacet:new(resetButton)
	resetButton.facets.label:setText("Reset")
	resetButton.facets.label:setSystemName("Text")
	resetButton.facets.label:createFacetInCore()
    resetButton.facets.interface = InterfaceFacet:new(resetButton)
	resetButton.facets.interface:setClickable(true)
	resetButton.facets.interface:setOnClick(
	    function(self, interfaceState)
            for index, slider in ipairs(sliders) do
                slider:reset()
            end
            Renderer.fadeWindow(3000, {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}, fadeCallback2)         
	    end
	)
    resetButton:setPosition({60, Config.window.dimensions[2] - 30,10})

    fadeButton = Entity:new()
    fadeButton:addBasicPosition("Positions")
	fadeButton:setDimensions({50,21})
	fadeButton.facets.label = TextFacet:new(fadeButton)
	fadeButton.facets.label:setText("Fade")
	fadeButton.facets.label:setSystemName("Text")
	fadeButton.facets.label:createFacetInCore()
    fadeButton.facets.interface = InterfaceFacet:new(fadeButton)
	fadeButton.facets.interface:setClickable(true)
	fadeButton.facets.interface:setOnClick(
	    function(self, interfaceState)       

            Renderer.fadeWindow(3000, {0.5,0.5,0.5,0, 0,0,0,0, 0,0,0,0, 0,0,0,1}, fadeCallback)
        
            --[[local fadeTransition = {
                typeName = "FadeTransition",
                entityId = rainbow:getId(),
                facetId = -1,
                duration = 3000,
                beginTransform = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
                endTransform = {0,1,0,0, 0,0,1,0, 1,0,0,0, 0,0,0,0.5},
                finishedCallback = fadeCallback
            }
            Transitions.createTransition(fadeTransition)
            ]]
	    end
	)
    
    fadeButton:setPosition({60, Config.window.dimensions[2] - 60,10})     
   
   quitButton = Entity:new()
   quitButton:addBasicPosition("Positions")
   quitButton.facets.label = TextFacet:new(quitButton)
   quitButton.facets.label:setSystemName("Text")
   quitButton.facets.label:setText("Quit")
   quitButton.facets.label:createFacetInCore()
   quitButton.facets.interface = InterfaceFacet:new(quitButton)
   quitButton.facets.interface:setClickable(true)
   quitButton.facets.interface:setOnClick(
    function(self, interfaceState)
        Core.quit("Quit")
    end
    )
    quitButton:setDimensions(quitButton.facets.label:getDimensions())
    quitButton:setPosition({60, Config.window.dimensions[2] - 90,10})

end


function Scenes.rainbow.update(dt)

end


function Scenes.rainbow.reset()

end