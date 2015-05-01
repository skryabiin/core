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




Core.setNextScene(Scenes.title)
	
while not doQuit do
	
	Console.info("Entering main loop.")
	
	Core.init()

	Core.transitionScene()
	
	local mainSequenceResponse = Core.run()	
	
	if mainSequenceResponse == nil then
		Console.info("Response is nil")
	else
		Console.info("main loop response is " .. mainSequenceResponse)
	end
	
	Core.reset()
	
	--if the user is trying to close the window, quit now
	if mainSequenceResponse == "AppClosed"  or mainSequenceResponse == "Quit" then
		Renderer.hideWindow()
		doQuit = true
	end

	--otherwise, this will continue to the next scene
	
end

Core.destroy()

