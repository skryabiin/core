PhysicsFacet = Facet:new()
	
function PhysicsFacet:new(entity, system, o)
	o = o or {}	
	o = self:create(entity, system, o)
    return o;
end
	
function PhysicsFacet:type()
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
	self._velocity = velocity
	local vce = {
			typeName = "VelocityChangeEvent",
			entityId = self:of():id(),
			axes = axes,
			additive = false,
			velocityChange = velocity
		}	
	EventProcessor.process(vce)		
end


function PhysicsFacet:createPhysicsFacet(systemName)	
	local facet = {
		position = self:position(),
		dimensions = self:dimensions(),
		velocity = self:velocity(),
		acceleration = self:acceleration(),
		mass = self:mass(),
		blocking = self:isBlocking(),
		fixed = self:fixed()
	}
	addPhysicsFacet_bind(systemName, self:of():id(), facet)
	
end

function PhysicsFacet:velocity()
	return self._velocity or {0.0,0.0}
end
	
function PhysicsFacet:setAcceleration(acceleration)
	self._acceleration = acceleration
end
	
	
function PhysicsFacet:acceleration()
	return self._acceleration or {0,0}
end

function PhysicsFacet:isBlocking()
	return self._isBlocking or false
end

function PhysicsFacet:setBlocking(blocking)
	self._isBlocking = blocking
end

function PhysicsFacet:isFixed()
	return self._isFixed or true
end

function PhysicsFacet:setFixed(fixed)
	self._isFixed = true
end

function PhysicsFacet:setMass(mass)
	self._mass = mass
end

function PhysicsFacet:mass()
	return self._mass or 0.0
end

function PhysicsFacet:createFacetInCore()
	if self:isCoreFacet() == false then
		local facet = {
			position = self:of():position(),
			dimensions = self:of():dimensions(),
			velocity = self:velocity(),
			acceleration = self:acceleration(),
			mass = self:mass(),
			blocking = self:isBlocking(),
			fixed = self:isFixed()
		}
		if self:of() == nil then
			Console.debug("self.of is nil in physics facet")
		end
		self._id = addPhysicsFacet_bind(self:system():name(), self:of():id(), facet)		
	end
end

function PhysicsFacet:updateFacetInCore()

end