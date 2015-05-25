#include "World.hpp"
#include "Core.hpp"
#include "LuaList.hpp"

namespace core {


	bool World::createImpl() {

		auto& lua = single<Core>().lua();
		lua.bindFunction("loadMap_bind", loadMap_bind);
		lua.bindFunction("runMap_bind", runMap_bind);
		lua.bindFunction("centerCameraAt_bind", centerCameraAt_bind);
		lua.bindFunction("cameraRoll_bind", cameraRoll_bind);
		lua.bindFunction("cameraPitch_bind", cameraPitch_bind);
		lua.bindFunction("cameraYaw_bind", cameraYaw_bind);
		lua.bindFunction("cameraMove_bind", cameraMove_bind);
		lua.bindFunction("cameraZoom_bind", cameraZoom_bind);

		_camera.create();
		return true;
	}

	bool World::initializeImpl() {

		for (auto& map : _maps) {
			map.second.initialize();
		}		

		_camera.initialize();			
		return true;

	}
	bool World::resetImpl() {
		for (auto& map : _maps) {
			map.second.reset();
		}
		_camera.reset();
		return true;
	}
	bool World::destroyImpl() {
		for (auto& map : _maps) {
			map.second.destroy();
		}
		_camera.destroy();
		_maps.clear();

		return true;
	}


	void World::addMap(Map map) {
		map.create();
		map.initialize();
		_maps.insert(std::pair<std::string, Map>(map.name(), map));
	}

	Map World::getMap(std::string mapName) {
		auto iter = _maps.find(mapName);

		if (iter == _maps.end()) {
			warn("Map '", mapName, "' not found!");

			return Map{};
		}
		else {
			return iter->second;
		}
	}

	void World::runMap(std::string mapName, LuaFunction tileParseCallback) {
		int runMapStart = SDL_GetTicks();
		Map map = getMap(mapName);

		info("Running map ", mapName);
		
		int parseMapStart = SDL_GetTicks();
		_tmxMap.ParseFile(map.tmxFilePath);

		int parseMapDuration = SDL_GetTicks() - parseMapStart;
		debug("Parse map took ", parseMapDuration, "ms");

		int r, g, b;
		auto c = Color{};
		if (!_tmxMap.GetBackgroundColor().compare("")) {
			r = 0;
			g = 0;
			b = 0;
		}
		else {
			hexToRgb(_tmxMap.GetBackgroundColor(), c.r, c.g, c.b);
		}
		c.r *= 1.0f / 255.0f;
		c.g *= 1.0f / 255.0f;
		c.b *= 1.0f / 255.0f;
		c.a = 1;

		single<Renderer>().setBackgroundColor(c);

		auto& lua = single<Core>().lua();
		const std::vector<Tmx::Layer*> layers = _tmxMap.GetLayers();
		int callbackDuration = 0;
		for (auto layer : layers) {

			int x1, y1;
			int u1, v1;
			int layerWidth = layer->GetWidth();
			int layerHeight = layer->GetHeight();

			
			// so much shit code
			
			for (int y = 0; y < layerHeight; y++) {
				for (int x = 0; x < layerWidth; x++) {

					int tileId = layer->GetTileId(x, layerHeight - y - 1);
					int tilesetId = layer->GetTileTilesetIndex(x, layerHeight - y - 1);

					if (tilesetId >= 0) {


						auto thisSet = _tmxMap.GetTileset(tilesetId);
						auto tile = _tmxMap.GetTileset(tilesetId)->GetTile(tileId);

						//figure out which section of the texture this tile is
						auto setw = (thisSet->GetImage()->GetWidth() / thisSet->GetTileWidth());
						auto seth = (thisSet->GetImage()->GetHeight() / thisSet->GetTileHeight());

						u1 = tileId % setw;
						v1 = (tileId - u1) / setw;
						u1 *= thisSet->GetTileWidth();
						v1 *= thisSet->GetTileHeight();

						x1 = x * _tmxMap.GetTileWidth();
						y1 = y * _tmxMap.GetTileHeight();



						auto tileInfo = TileInformation{};
						tileInfo.position.setPixel(Pixel{ x1, y1, layer->GetZOrder() });
						tileInfo.dimensions.setDimension(Dimension{ thisSet->GetTileWidth(), thisSet->GetTileHeight() });
						tileInfo.sourceTexturePosition.setPixel(Pixel{ u1, v1, 0 });

						tileInfo.textureName = map.tilesets[thisSet->GetName()];
						auto& tileProperties = tile->GetProperties();
						tileInfo.tileProperties = tileProperties.GetList();

						int beforeCallbackTicks = SDL_GetTicks();

						lua.call(tileParseCallback, tileInfo);

						callbackDuration += SDL_GetTicks() - beforeCallbackTicks;


					}

				} //end layer X loop

			} //end layer Y loop

		} //end layers loop
		int runMapDuration = SDL_GetTicks() - runMapStart;
		debug("Callbacks took ", callbackDuration, "ms");
		debug("Entire load took ", runMapDuration, "ms");
		
	}



	Camera* World::camera() {
		return &_camera;
	}

	int World::loadMap_bind(LuaState& lua) {

		auto map = Map{};

		map.setName(lua["name"]);

		map.tmxFilePath = lua["tmxFilePath"];

		int len = lua["tilesets"].length();

		std::string tilesetName;
		std::string spriteName;
		for (int i = 1; i <= len; ++i) {
			tilesetName = lua["tilesets"][i]["name"];
			spriteName = lua["tilesets"][i]["spriteName"];
			map.tilesets[tilesetName] = spriteName;
		}

		single<World>().addMap(map);


		return 0;
	}

	int World::centerCameraAt_bind(LuaState& lua) {

		auto position = LuaPixel{};
		position.fromLua(lua);

		single<World>().camera()->setPosition(position.getPixel());

		return 0;
	}

	int World::cameraRoll_bind(LuaState& lua) {

		float degrees = lua.pullStack<float>();

		single<World>().camera()->roll(degrees);
		return 0;
	}

	int World::cameraPitch_bind(LuaState& lua) {

		float degrees = lua.pullStack<float>();

		single<World>().camera()->pitch(degrees);

		return 0;
	}

	int World::cameraYaw_bind(LuaState& lua) {

		float degrees = lua.pullStack<float>();

		single<World>().camera()->yaw(degrees);

		return 0;
	}

	int World::cameraMove_bind(LuaState& lua) {

		auto move = LuaVec2{};
		move.fromLua(lua);						

		single<World>().camera()->move(move.getVec2());

		return 0;
	}

	int World::cameraZoom_bind(LuaState& lua) {
	
		auto zoom = LuaVec2{};
		zoom.fromLua(lua);

		single<World>().camera()->setZoom(zoom.getVec2());

		return 0;
	}

	int World::runMap_bind(LuaState& lua) {
		std::string mapName = lua.pullStack<std::string>(1);
		LuaFunction tileParseCallback = lua.pullStack<LuaFunction>(2);

		single<World>().runMap(mapName, tileParseCallback);
		return 0;
	}

} //end namespace core