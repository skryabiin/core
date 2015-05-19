
PhysicsEntity = Entity:new()

function PhysicsEntity:doCopy()
	return PhysicsEntity:new()
end

function PhysicsEntity:new(system, o)	
	o = self:create(o)
	o.facets.physics = PhysicsFacet:new(system, o)    
	return o
end


