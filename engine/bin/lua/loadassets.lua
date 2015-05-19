

Assets = {}


function Assets.load() 
	
	Fonts.load(Assets.fonts)
	Audio.load(Assets.sounds)
	Maps.load(Assets.maps)
	Sprites.load(Assets.sprites)
	ParticleEffects.load(Assets.particleEffects)
	VertexShaders.load(Assets.vertexShaders)
	FragmentShaders.load(Assets.fragmentShaders)
	
	
end


Assets.vertexShaders = {

	{
		name = "basic",
		path = "./shaders/basic.glvs",
		attributes = {
			"vertexPos"
		},
		uniforms = {}
	},
	{
		name = "shape2d",
		path = "./shaders/shape2d.glvs",
		attributes = {
			"vertexPos"
		},
		uniforms = {}
	},
	{
		name = "textureRender2d",
		path = "./shaders/textureRender2d.glvs",
		attributes = {
			"vertexPos",
			"uvIn"
		},
		uniforms = {
            "mvp"
        }
	},
    {
		name = "finalPass2d",
		path = "./shaders/finalPass2d.glvs",
		attributes = {
			"vertexPos",
			"uvIn"
		},
		uniforms = {

        }
	}
}

Assets.fragmentShaders = {

	{
		name = "basic",
		path = "./shaders/basic.glfs",
		uniforms = {
			"drawColor"
		}
	},
	{
		name = "shape2d",
		path = "./shaders/shape2d.glfs",
		uniforms = {
			"drawColor",
            "colorTransform"
		}
	},
	{
		name = "textureRender2d",
		path = "./shaders/textureRender2d.glfs",
		uniforms = {
			"textureSampler",
			"colorTransform"
		}
	},
    {
        name="finalPass2d",
        path="./shaders/finalPass2d.glfs",
        uniforms = {
            "textureSampler",
            "globalTransform"
        }
    }
}


Assets.particleEffects = {

	snow = {
		
		systemType = "linear",
		numParticlesStart = 500,
		numParticlesMax = 500,
		
		colorRangeR = {255, 255},
		colorRangeG = {255, 255},
		colorRangeB = {255, 255},
		colorRangeA = {255, 255},
		accelXRange = {0.0, 400.0},
		accelYRange = {0.0, 400.0},
		velocityXRange = {200.0, 400.0},
		velocityYRange = {200.0, 400.0},
		initialDelayRange = {0, 1000},
		lifetimeRange = {500, 600},
		particleWidthRange = {2, 4},
		particleHeightRange = {2, 4},
		fieldZone = {0, 0, Config.window[1], Config.window[2]},
		spawnZone = {0, 0, Config.window[1], Config.window[2]},
		startInMediaRes = false,
		endOnZoneClip = true,
		fadeOut = true
	}
}

Assets.fonts = {
	{
		default = true,
		renderedSize = 15,
		name="RobotoCondensed-Regular",
		path="./lua/fonts/RobotoCondensed-Regular.ttf"
	},
	{ 
		default = false,
		renderedSize = 30,
		name = "PixelBerry",			
		path = "./lua/fonts/pixel_berry.ttf"
	}
}
			
Assets.sounds = {
	{
		name = "DoorOpen",
		path = "./lua/sounds/button-25.wav"
	}, 
	{
		name = "RobotJump",
		path = "./lua/sounds/button-3.wav"
	}
}

Assets.maps = {
	{
		name = "Level1",
	    tmxFilePath = "./lua/maps/level1/Level.tmx",
        tilesets = {            
            {
                name="palette",
                spriteName="level1tileset"
            },
            {
                name="guy",
                spriteName="level1guy"
            }
        }
    }
}

	
Assets.sprites = {

    level1guy = {
        name = "level1guy",
        path = "./lua/maps/tilesets/guy.png"
    },
    level1tileset = {
        name = "level1tileset",
        path = "./lua/maps/tilesets/Palette2.png"
    },
    rainbow = {
        name = "rainbow",
        path = "./lua/sprites/6807326-rainbow.jpg"
    },

	redBox = { 
		name = "redBox",
		path = "./lua/sprites/red_box.png"
	},
	
	blueBalls = {
		name = "blueBalls",
		path = "./lua/sprites/AUAlogowithRibbon.png"
	},
	
	cursor_brown = {
		name = "cursor_brown",
		path = "./lua/sprites/cursor_brown.png"
	},
	
	Brobot = { 
		name = "Brobot",
		path = "./lua/sprites/brobot/littlebot.png",
		animations = {
			{
				name = "StandRight",
				firstLoopFrame = 0,
				orientation = {1, 0},
				frames = {
					{0, 0, 64, 128, 200}
				}
			},
			{
				name = "StandLeft",
				firstLoopFrame = 0,
				orientation = {-1, 0},
				frames = {
					{0, 128, 64, 128, 200}
				}
			},
			{
				name = "MoveRight",
				firstLoopFrame = 0,
				orientation = {1, 0},
				frames = {
					{64, 0, 64, 128, 200},
					{0, 0, 64, 128, 200},
					{192, 0, 64, 128, 200},
					{128, 0, 64, 128, 200}
				}
			},
			{
				name = "MoveLeft",
				firstLoopFrame = 0,
				orientation = {-1, 0},
				frames = {
					{64, 128, 64, 128, 200},
					{0, 128, 64, 128, 200},
					{192, 128, 64, 128, 200},
					{128, 128, 64, 128, 200}						
				}
			}
		}
	}	
}	
