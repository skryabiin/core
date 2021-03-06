

Fonts = {}


function Fonts.loadFont(v)
Console.info("Loading font: '", v.name, "', ", v.path, ", default size ", v.defaultSize)
	loadFont_bind(v.name, v.path, v.defaultSize, v.default, true)
end

function Fonts.getDefaultFont()
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


