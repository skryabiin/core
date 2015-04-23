
Sprites = {}

function Sprites.loadAnimationSprite(v)
	Console.info("Loading animation set: '", v.name, "', ", v.path)
	loadAnimationSet_bind(v)

end

function Sprites.loadTextureSprite(v)
	Console.info("Loading texture: '", v.name, "', ", v.path)
	loadTexture_bind(v)
end

function Sprites.load(sprites)
Console.info("Loading sprites...")
Sprites.sprites = sprites
	for i, v in pairs(sprites) do 
		if v.animations then
			Sprites.loadAnimationSprite(v)
		else
			Sprites.loadTextureSprite(v)
		end
	end
end

