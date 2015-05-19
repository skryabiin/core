

Fonts = {}


function Fonts.loadFont(v)
Console.info("Loading font: '", v.name, "', ", v.path, ", size ", v.renderedSize)
	loadFont_bind(v.name, v.path, v.renderedSize, v.default, true)
end

function Fonts.defaultFont()
	for i, v in ipairs(Fonts.fonts) do
		if v.default then
			return v
		end
	end
end
			
function Fonts.load(fonts)
	Console.info("Loading fonts...")
	
	Fonts.fonts = fonts
	
	for i, v in ipairs(fonts) do Fonts.loadFont(v) end
end


