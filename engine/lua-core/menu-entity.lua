
MenuEntity = Entity:new()

function MenuEntity:new(o)
	o = self:create(o)
end

function MenuEntity:setName(name)
	self.name = name
end

function MenuEntity:setBackgroundTextureName(textureName)
	self.backgroundTextureName = textureName
end

function MenuEntity:setBackgroundColorMod(backgroundColorMod)
	self.backgroundColorMod = backgroundColorMod
end


