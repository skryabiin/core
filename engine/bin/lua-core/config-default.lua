--Configuration settings
Config = {}

--Settings for the actual window
Config.window = {}

--The title of the game window
Config.window.title = "Default"

--Window dimensions in pixels, W x H
Config.window.dimensions = {1024, 768}

--Whether to display the window in full-screen
Config.window.fullscreen = false

--The following Config.window settings apply only when Config.window.fullscreen is false
--whether the window should be centered when created
Config.window.centered = true

--Initial position of the window
--Only applies if Config.window.centered = false
Config.window.position = {0,0}

--Whether the user can resize the window
Config.window.resizable = true

--Whether the window will have a traditional border
Config.window.borderless = false

--Whether the mouse will be constrained to the window
Config.window.grabMouse = false



--Logging and debugging settigns
Config.logging = {}
--log file path
Config.logging.logFile = "log.txt"

--See constants.lua for verbosity level constants
Config.logging.verbosityLevel = Constants.VERBOSITY_LEVEL_DEBUG



--default camera settings
Config.camera = {}
--Default camera center world point
Config.camera.centerWorldPoint = {512, 384}

--Default camera scaling
Config.camera.scale = {1.0, 1.0}



--System settings used by the engine
Config.system = {}

--The maximum logic update loop rate
--
--Setting this to a reasonable number reduces load
--on the CPU
Config.system.maxUpdatesPerSecond = 200

--Does memory integrity checking
--
--This will have a significant performance impact,
--and should only be used for specific debugging purposes
Config.system.debugMemory = false



--Graphics settings, performance focussed
Config.graphics = {}
--The maximum frame rate of the OpenGL render thread
--
--Setting this to a reasonable number reduces load
--on the GPU
Config.graphics.maxFramesPerSecond = 100

--Set this to true to enable multithreaded rendering
--
--Generally "true" will result in better performance,
--But may make debugging more difficult
Config.graphics.renderMultithreaded = false

--The depth that the main thread will allow the current
--drawable update writing queue to reach before waiting 
--for the render thread to drain the reading queue.
--
--this is used in situations where the update thread significantly
--outpaces the render thread
--
--Only applies in multithreaded render mode
Config.graphics.maxWaitFreeQueueDepth = 10

--Logs debug messages from OpenGL 
--
--This will have some performance impact
Config.graphics.debugOpenGl = true



--Audio settings
Config.audio = {}

--The playback rate for audio
--
--44100 is CD quality, but CPU intensive
--22050 is supposedly what most games use
Config.audio.samplingFrequency = 22050

--"true" for stereo, "false" for mono
Config.audio.stereo = true

--Sample buffer size
--
--On a slow system, if this is too small you
--may encounter some audio skipping
--If this is too large, runtime audio calls 
--(such as "play sound" or "change volume")
--will lag
--
--4096 is ok for just music
--2048 seems to be a happy medium
Config.audio.chunkSize = 2048