PhysicsFacet = Facet:new()
	
function PhysicsFacet:new(entity, systemName, o)
	o = o or {}	
	o = self:create(entity, systemName, o)
    return o;
end
	
function PhysicsFacet:getType()
	return "PhysicsFacet"
end

function PhysicsFacet:setVelocity(velocity)

	local axes = 0	
	if velocity[1] ~= nil then 
		axes = axes - 1
	else
		velocity[1] = 0.0
	end
	
	if velocity[2] ~= nil then 
		axes = axes + 1
	else
		velocity[2] = 0.0
	end	
	self.velocity = velocity
	local vce = {
			typeName = "VelocityChangeEvent",
			entityId = self.of:getId(),
			axes = axes,
			additive = false,
			velocityChange = velocity
		}	
	EventProcessor.process(vce)		
end


function PhysicsFacet:createPhysicsFacet(systemName)	
	local facet = {
		position = self:getPosition(),
		dimensions = self:getDimensions(),
		velocity = self:getVelocity(),
		acceleration = self:getAcceleration(),
		mass = self:getMass(),
		blocking = self:getBlocking(),
		fixed = self:getFixed()
	}
	addPhysicsFacet_bind(systemName, self.of:getId(), facet)
	
end

function PhysicsFacet:getVelocity()
	return self.velocity or {0.0,0.0}
end
	
function PhysicsFacet:setAcceleration(acceleration)
	self.acceleration = acceleration
end
	
	
function PhysicsFacet:getAcceleration()
	return self.acceleration or {0,0}
end

function PhysicsFacet:isBlocking()
	return self.isBlocking or false
end

function PhysicsFacet:setBlocking(blocking)
	self.isBlocking = blocking
end

function PhysicsFacet:isFixed()
	return self.isFixed or true
end

function PhysicsFacet:setFixed(fixed)
	self.isFixed = true
end

function PhysicsFacet:setMass(mass)
	self.mass = mass
end

function PhysicsFacet:getMass()
	return self.mass or 0.0
end

function PhysicsFacet:createFacetInCore()
	if self:isCoreFacet() == false then
		local facet = {
			position = self.of:getPosition(),
			dimensions = self.of:getDimensions(),
			velocity = self:getVelocity(),
			acceleration = self:getAcceleration(),
			mass = self:getMass(),
			blocking = self:isBlocking(),
			fixed = self:isFixed()
		}
		if self.of == nil then
			Console.debug("self.of is nil in physics facet")
		end
		self.id = addPhysicsFacet_bind(self.systemName, self.of:getId(), facet)		
	end
end

function PhysicsFacet:updateFacetInCore()

end