
PhysicsEntity = Entity:new()

function PhysicsEntity:doCopy()
	return PhysicsEntity:new()
end

function PhysicsEntity:new(o)	
	o = self:create(o)
	o.facets.physics = PhysicsFacet:new(o)
	return o
end


