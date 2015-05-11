
PhysicsEntity = Entity:new()

function PhysicsEntity:doCopy()
	return PhysicsEntity:new()
end

function PhysicsEntity:new(systemName, o)	
	o = self:create(o)
	o.facets.physics = PhysicsFacet:new(systemName, o)    
	return o
end


