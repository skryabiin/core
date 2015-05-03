require "lua-core/require"
require "lua/config"


--------------------------------------
--this is where the application script begins
--
--at this point, SDL is initialized, the Config.window is created, etc.
--------------------------------------


--initializes the engine
Core.create()
	

--set this up for the sequence loop
require "lua/menus"
require "lua/firstscene"
require "lua/titlescene"
require "lua/loadassets"

Assets.load()


local cursorTexture = {
			source = {0, 0, 24, 32},
			dimensions = {24, 32},
			textureName = "cursor_brown",			
			scale = {1, 1},
			offset = {0, -32},			
			textureColorMod = {100,255,255,100}
		}
        


Core.setNextScene(Scenes.title)
	
while not doQuit do
	
	Console.info("Entering main loop.")
	
	Core.init()

    Interface.setCursorTexture(cursorTexture)

	Core.transitionScene()
	
	local mainSequenceResponse = Core.run()	
	
	if mainSequenceResponse == nil then
		Console.info("Response is nil")
	else
		Console.info("main loop response is " .. mainSequenceResponse)
	end
	
	Core.reset()
	
	--if the user is trying to close the window, quit now
	if mainSequenceResponse == "AppClosed"  or mainSequenceResponse == "Quit" or mainSequenceResponse == "Fatal" then
		Renderer.hideWindow()
		doQuit = true
	end

	--otherwise, this will continue to the next scene
	
end

Core.destroy()

