


local tile = {
	position = {0,0},
	dimensions = {0,0},
	sourceTexturePosition = {0,0},
	textureName = "TextureSomething",
	tileProperties = { propertyName = "value" }
}



world = {
	
	--opens a TMX map by name, and calls the user-supplied objectParserFunction for each tile
	openMap = function(mapName, objectParserFunction)
	
		
		
		if currentWorldSystemPtr == nil then
			currentWorldSystemPtr = Core.createSystem("WorldSystem", "World")
			currentPhysicsSystemPtr = Core.createSystem("PhysicsSystem", "Physics")
			currentTextureRenderSystemPtr = Core.createSystem("TextureRenderSystem", "Textures")
			currentAnimationSystemPtr = Core.createSystem("AnimationSystem", "Animations")			
			currentTextRenderSystemPtr = Core.createSystem("TextRenderSystem", "Text")

		else
			--local physics = Core.getSystem("Physics")
			--physics.clear()
			
			--local animations = Core.getSystem("Animations")
			--animations.clear()
			
			--local textures = Core.getSystem("Textures")
			--textures.clear()
			
			--Core.cleanupEntities()
			
		end
		
		--openMap_bind("World", mapName, objectParserFunction)
		
		--Core.callSystemBinding(worldSystem, "OpenMap", mapName, objectParserFunction)
		
	end,
	
	--adds a tile to the world, based on the tmx tile location and tileset entry.
	--a non-blocking tile of this calibur will not participate in collision detection
	--a blocking tile of this calibur will participate in collision detection, but 
	--will publish only a limited set of events
	addTileFromTileset = function(tile, blocking, fixed)
		
		local entity = Core.createEntity()
		
		local physicsFacet = {
				position = tile.position,
				dimensions = tile.dimensions,
				velocity = {0.0, 0.0},
				acceleration = {0.0, 0.0},
				mass = 1.0,
				blocking = blocking,
				fixed = fixed
			}
		
		addPhysicsFacet_bind("Physics", entity.id, physicsFacet)	
		
		local textureFacet = {
			position = tile.position,
			dimensions = tile.dimensions,
			textureName = tile.textureName,
			sourceTextureRect = tile.sourceTextureRect,
			scale = {1.0, 1.0},
			offset = {0,0,0}
		}
		
		addTextureFacet_bind("Textures", entity.id, textureFacet)

		return entity
	end,

	addObjectFromTmxObject = function(tile, blocking, fixed)
	
		local entity = Core.createEntity()
		

		local physicsFacet = {
			position = tile.position,
			dimensions = tile.dimensions,
			velocity = {0.0, 0.0},
			acceleration = {0.0, 0.0},
			mass = 1.0,
			blocking = blocking,
			fixed = fixed
		}
	
		addPhysicsFacet_bind("Physics", entity.id, physicsFacet)			
		Console.debug("creating physics for entity ", entity.id)
		Console.debug("position is ", tile.position[1], ", ", tile.position[2])
				
	return entity
	
	end,
	
	--adds a tile to the world in an arbitrary position, with the given sprite as image
	--the same rules apply for collision as in addTileFromTileset()
	addTileFromSprite = function(position, spriteName, blocking)

	
	end,
	
	
	--adds a portal to the world based on the tile location, tileset, and tmx object properties
	addPortalFromTileset = function(tile)
	
	end,

	--adds a portal to the world at an arbitrary position, wiht the given sprite as image
	--the linkedPortalId and linkedMapName are not validated, so beware!
	addPortalFromSprite = function(position, spriteName, linkedPortalId, linkedMapName)
	
	end,
	
	--adds an item to the world based on the tile location, tileset, and tmx object properties
	addItemFromTileset = function(tile)
	
	end,
	
	--adds an item to the world at an arbitrary position, with the given sprite as image
	addItemFromSprite = function(position, spriteName, itemType)
	
	end,

	--adds an entity to the world based on the tile location, tileset, and tmx object properties
	addEntityFromTileset = function(tile, entityType)
		
	end,
	
	--adds an entity to the world at an arbitrary position, with the given sprite as image
	addEntityFromSprite =  function(position, dimensions, scale, offset, spriteName, startingAnimation)
	
		Console.info("Adding entity from sprite '", spriteName, "'")
		local entity = Core.createEntity()
		Console.info("Entity id is ", entity.id)
		
		local physicsFacet = {
				position = position,
				dimensions = dimensions,
				velocity = {0.0, 0.0},
				acceleration = {0.0, 0.0},
				mass = 1.0,
				blocking = true,
				fixed = false
		}

		local animationFacet = {
			position = position,
			scale = scale, 
			animationSetName = spriteName,
			currentAnimation = startingAnimation,
			offset = offset
		}
		
		addAnimationFacet_bind("Animations", entity.id, animationFacet)
		Console.info("Created animation facet for entity ", entity.id)
		addPhysicsFacet_bind("Physics", entity.id, physicsFacet)
		Console.info("Created physics facet for entity ", entity.id)
		return entity
		
	end,

	--sets the gravity for the world.  this is a numeric vector in the form {0.0, -9.8}
	setGravity = function(gravity)
	
	end,
	
	doCameraFollow = function(entity)
	
		addCameraFollowFacet_bind("Textures", entity.id)
		addCameraFollowFacet_bind("Animations", entity.id)
		--addCameraFollowFacet_bind("Text", entity.id)
		
	end,
	
	currentWorldSystemPtr = nil,
	currentPhysicsSystemPtr = nil,
	currentAnimationSystemPtr = nil,
	currentTextureRenderSystemPtr = nil
	
}  -- end world