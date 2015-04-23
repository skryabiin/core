
pauseMenu = {
	
	menuActivateKey = keyScancodePause,
	
	parseInput = function(event)
				if event.down and (event.repeatCount == 0) then
					if pauseMenu.isActive then
						if event.keysym == keyScancodePause then
							pauseMenu.fadeOut()
						elseif event.keysym == sdl.key.up then
							pauseMenu.currentOption = pauseMenu.currentOption - 1
						elseif event.keysym == sdl.key.down then
							pauseMenu.currentOption = pauseMenu.currentOption + 1
						elseif event.keysym == sdl.key.space then
							pauseMenu.selectOption()
						end
					else
						if event.keysym == keyScancodePause then
							pauseMenu.fadeIn()
						end
					end
				end
			end,
				
	
	init = function() 
		EventProcessor.addEventFilter(0, "KeyboardEvent", parseInput)
		
	end,
	
	fadeIn = function()
	
	
	end,
	
	fadeOut = function()
	
	end,
	
	
	options = {
		{
			name = "Resume",
			entity = nil
		},
		{
			name = "Quit",
			entity = nil
		}
	}
}