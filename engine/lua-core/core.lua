
--require "lua-Core/menus"

Scenes = {}

Core = { 
	entities = {},
	controlFocus = 1,
    firstInit = true
}

function Core.init()
        if Core.firstInit then  
            Core.firstInit = false
            Console.setDebug()
        end
		return init_bind()
end

	
function Core.run()
		return run_bind()
end

function Core.quit(message)		
		doQuit_bind(message)
end
	
function Core.createEntity(entity)
		local entityId = createEntity_bind()
		entity.id = entityId
		Core.entities[entityId] = entity	
end
	
function Core.destroyEntity(entityId)
		destroyEntity_bind(entityId)
		Core.entities[entityId] = nil
end
	
function Core.reset() 
	reset_bind()
end

function Core.transitionScene()
	if Core.scene then
		Core.scene.reset()
	end
	Core.scene = Core.nextScene
	Core.scene.init()
end

function Core.setNextScene(nextScene)
	Core.nextScene = nextScene
end

function Core.createSystem(systemType, systemName, layerId)
	local passLayerId = layerId
	if passLayerId == nil then
		passLayerId = 0
	end
		createSystem_bind(systemType, systemName, passLayerId)
end
	
function Core.destroySystem(systemName)
	
end
	
function Core.pauseSystem(systemName)
	
end
	
function Core.callSystemBinding(systemType, systemName, boundFunctionName, ...)
	
end
	
function Core.playSound(soundName) 
		playSound_bind(soundName)
end
	


Entity = {}
	

function Entity:create(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	Core.createEntity(o)
	o.children = {}	
	o.facets = {}
	o.facets.interface = InterfaceFacet:new(o)
	o.positionFacetHandled = false
	return o	
end

function Entity:new(o)
	return Entity:create(o)	
end

function Entity:addBasicPosition(systemName)
	if not self.positionFacetHandled then
		local positionFacet = { 
			position = self:getPosition(),
			dimensions = self:getDimensions(),
			orientation = self:getOrientation()
		}
		self.positionFacetId = addPositionFacet_bind(systemName, self:getId(), positionFacet)
		self.positionFacetHandled = true
	end
end


function Entity:getChildren()
	return self.children or {}
end

function Entity:getParent()
	return self.parent or {}
end

function Entity:addChild(child) 
	self.children[child:getId()] = child
	child:setParent(self)
end

function Entity:removeChild(child)
	self.children[child:getId()] = nil
end

function Entity:setParent(parent)
	self.parent = parent
	if parent.children[self:getId()] == nil then
		parent:addChild(self)
	end
end


function Entity:setSprite(sprite)
		local animationChangeEvent = {
		typeName = "AnimationChangeEvent",
			entityId = self.id,
			nextAnimation = spriteName,
			endImmediate = true
		}
		EventProcessor.process(animationChangeEvent)	
end


function Entity:getId()
	return self.id
end

function Entity:setVisualScale(scale)
		self.scale = scale
end

function Entity:setColorMod(color)
		self.colorMod = color
end

function Entity:setRotation(angle)
		self.rotation = angle
end

function Entity:setFlip(flipX, flipY)
		self.flip = self.flip or {}
		self.flip.x = flipX
		self.flip.y = flipY
end

function Entity:align(horizontal, vertical)

end


function Entity:doFullPositionQuery() 
		local positionQuery = {
			typeName = "EntityPositionQuery",
			entityId = self.id,
			position = {0, 0, 0},
			dimensions = {0, 0},
			orientation = {0, 0}
		}		
		return EventProcessor.process(positionQuery)
end

function Entity:setPositionFacetHandled(handled)
	if not self:isPositionFacetHandled() and handled then
		local position = self:getPosition()
		local dimensions = self:getDimensions()
		local orientation = self:getOrientation()
		self.positionFacetHandled = true
		self:setPosition(position)
		self:setDimensions(dimensions)
		self:setOrientation(orientation)
	end
	self.positionFacetHandled = handled
end

function Entity:isPositionFacetHandled()
	return self.positionFacetHandled or false
end

function Entity:isPaused()
	return self.paused
end

function Entity:togglePause()
	self:setPaused(not self.paused)
end

function Entity:pause()
	self:setPaused(true)	
end

function Entity:resume()
	self:setPaused(false)
end

function Entity:setPaused(paused)
	local facetPauseEvent = {
			typeName = "FacetPauseEvent",
			entityId = self:getId(),
			paused = paused,
			facetId = -1}
	EventProcessor.process(facetPauseEvent)
	self.paused = paused
	
	for i, v in pairs(self:getChildren()) do		
		v:setPaused(paused)
	end
	
	for i, v in ipairs(self.facets) do
		v.paused = paused
	end
	

end

function Entity:getPosition()
	if not self:isPositionFacetHandled() then
		return self.position or {0,0,0}
	else
		local query = self:doFullPositionQuery()
		return query.position
	end
end

function Entity:getDimensions()
	if not self:isPositionFacetHandled() then
		return self.dimensions or {0,0}
	else
		local query = self:doFullPositionQuery()
		return query.dimensions
	end
end
function Entity:getOrientation()
	if not self:isPositionFacetHandled() then
		return self.orientation or {1,0}
	else
		local query = self:doFullPositionQuery()
		return query.orientation
	end
end

function Entity:setPosition(position)	
	if not self:isPositionFacetHandled() then
		self.position = position		
	else 
		local positionChangeEvent = {
			typeName = "PositionChangeEvent",
			entityId = self.id,
			position = position,
			relative = false
		}
		EventProcessor.process(positionChangeEvent)
	end
end

function Entity:setDimensions(dimensions)
	if not self:isPositionFacetHandled() then
		self.dimensions = dimensions
	else 
		local dimensionChangeEvent = {
			typeName = "DimensionChangeEvent",
			entityId = self.id,
			dimensions = dimensions		
		}		
		EventProcessor.process(dimensionChangeEvent)		
	end
end

function Entity:setOrientation(orientation)
	if not self:isPositionFacetHandled() then
		self.orientation = orientation
	else		
		local orientationChangeEvent = {
			typeName = "OrientationChangeEvent",
			entityId = self.id,			
			orientation = orientation	
		}
		EventProcessor.process(orientationChangeEvent)
	end
end

function Entity:addEventFilter(eventType, callbackFunction)

	EventProcessor.addEventFilter(self.id, eventType, callbackFunction)

end



Console = { }

function Console.setDebug()
		setDebug_bind()
end
function Console.debugfunction(...)
		msg = Console.out(...)
		debug_bind(msg)
end
	
function Console.info(...)
		msg = Console.out(...)
		info_bind(msg)
end

function Console.warn(...)
		msg = Console.out(...)
		warn_bind(msg)
end

function Console.error(...)
		msg = Console.out(...)
		error_bind(msg)
end
	
function Console.out(...)
		local d = debug.getinfo(3)
		local src = string.gsub(d.short_src, "(.*)/", "")
		local str = src .. "(" .. d.currentline .. ")"
		
		if d.name ~= nil then 
			str = str .. "," .. d.name
		end
		
		str = str .. ": " .. Console.outRec(...)
		
		return str
	
end
	
function Console.outRec(...)
		
		local msgs = table.pack(...)
		if #msgs > 0 then 
			local msg = table.remove(msgs, 1) or "nil"	
			if msg == true then msg = "true" end
			if msg == false then msg = "false" end
			return msg .. Console.outRec(table.unpack(msgs))
		else return "" end
	end


local textureFacet = {
	source = {100, 200},
	dimensions = {32, 32},
	texture = "textureName",
	position = {0,0, 0},	
	scale = {1.0, 1.0},
	offset = {0, 0}
}


local physicsFacet = {

	position = {0, 0, 0},
	dimensions = {32, 32},
	velocity = {0.0, 0.0},
	acceleration = {0.0, 0.0},
	mass = 1.0,
	blocking = true,
	fixed = false
}

local animationFacet = {

	position = {0, 0, 0},
	scale = {1.0, 1.0},
	offset = {0, -20},
	animationSetName = "myAnimationSet",
	currentAnimation = "StandLeft",
	orientation = {-1.0, -1.0 }
	
}

local textFacet = {
	position = {0, 0, 0},
	offset = {0, -20},
	font = "default",
	text = "testing text facets",
	color = {255, 255, 255, 255}
}


EventProcessor = { }

function EventProcessor.addEventFilter(entityId, eventType, callbackFunction)
		return addEventFilter_bind(entityId, eventType, callbackFunction)
end

function EventProcessor.removeEventFilter(filterId)
		removeEventFilter_bind(filterId)
end

function EventProcessor.process(event)
		return processEvent_bind(event.typeName, event)		
end
	
function EventProcessor.scheduleTimedEvent(delay, entityId, eventName, callbackFunction) 
		scheduleTimedEvent_bind(delay, entityId, eventName, callbackFunction)		
end
	
local collisionEvent = {
	typeName = "CollisionEvent",
	entityId = 123,
	newPosition = {0, 100, 0},
	newVelocity = {0.0, 1.0},
	contacts = {
		{ 
			entityId = 456,
			contactNormal = {1.0, 0.0},
			blocking = true,
			state = -1
		},
		{
			entityId = 567,
			contactNormal = {0.0, 1.0},
			blocking = false,
			state = 1
		}
	}
}

local animationChangeEvent = {
	typeName = "AnimationChangeEvent",
	entityId = 123,
	nextAnimation = "StandLeft",
	endImmediate = true
}

local entityPositionQuery = {
	typeName = "EntityPositionQuery",
	entityId = 123,
	position = {100, 100, 0}
}


local keyboardEvent = {
	typeName = "KeyboardEvent",
	keysym = 456,
	scancode = 123,
	down = true,
	repeatCount = 0
}

local gamepadEvent = {
	typeName = "GamepadEvent",
	button = 123,
	up = true
}

local velocityChangeEvent = {

	typeName = "VelocityChangeEvent",
	entityId = 123,
	axes = -1,
	additive = false,
	velocityChange = {-400.0, 0.0 }
}
	
local positionChangeEvent = {
	typeName = "PositionChangeEvent",
	entityId = 123,
	position = {100,200},
	orientation = {1.0, 0.0}
}

local textContentChange = {
	typeName = "TextContentChange",
	entityId = 123,
	textContent = "My new text content"
}