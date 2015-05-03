Config = {}

--Default camera center world point
Config.centerWorldPoint = {400, 300}

--Default camera scaling
Config.scale = {1.0, 1.0}

--not used
Config.keyDebugToggle =  96
Config.buttonPause = 6
Config.keyScancodePause = 0		

--The title of the game window
Config.title = "Default"

--Window dimensions in pixels, W x H
Config.window = {1024, 768}

--Whether to display the window initially
--Temporarily not used
Config.window.show = true

--log file path
Config.logFile = "log.txt"

--See constants.lua for verbosity level constants
Config.verbosityLevel = Constants.VERBOSITY_LEVEL_DEBUG


--These are settings that affect performance or other
--runtime behavior of the engine
PerformanceConfig = {}


--The maximum logic update loop rate
--
--Setting this to a reasonable number reduces load
--on the CPU
PerformanceConfig.maxUpdatesPerSecond = 200

--The maximum frame rate of the OpenGL render thread
--
--Setting this to a reasonable number reduces load
--on the GPU
PerformanceConfig.maxFramesPerSecond = 100


--Set this to true to enable multithreaded rendering
--
--Generally "true" will result in better performance,
--But may make debugging more difficult
PerformanceConfig.renderMultithreaded = true

--The depth that the main thread will allow the current
--drawable update writing queue to reach before waiting 
--for the render thread to drain the reading queue.
--
--this is used in situations where the update thread significantly
--outpaces the render thread
--
--Only applies in multithreaded render mode
PerformanceConfig.maxWaitFreeQueueDepth = 50