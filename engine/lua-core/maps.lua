
Maps = {}

function Maps.parseMapObject()

end
	
function Maps.loadMap(map)
	Console.info("Loading map: '", map.name, "', ", map.path)
	loadMap_bind(map)
end

function Maps.load(maps)
	Console.info("Loading maps...")
	Maps.maps = maps
	for i, v in ipairs(maps) do Maps.loadMap(v) end
end

