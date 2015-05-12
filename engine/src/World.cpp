#include "World.hpp"
#include "Core.hpp"

namespace core {


	bool World::createImpl() {

		auto& lua = single<Core>().lua();
		lua.bindFunction("loadMap_bind", loadMap_bind);
		_camera.create();
		return true;
	}

	bool World::initializeImpl() {

		for (auto& map : _loadedMaps) {
			map.second.initialize();
		}		
		
		_camera.initialize();
		_camera.roll(90.0f);
		_camera.moveOut(5.0f);
		return true;

	}
	bool World::resetImpl() {
		for (auto& map : _loadedMaps) {
			map.second.reset();
		}
		_camera.reset();
		return true;
	}
	bool World::destroyImpl() {
		for (auto& map : _loadedMaps) {
			map.second.destroy();
		}
		_camera.destroy();
		_loadedMaps.clear();

		return true;
	}


	void World::addMap(Map map) {
		map.create();
		map.initialize();
		_loadedMaps.insert(std::pair<std::string, Map>(map.name(), map));
	}

	Map World::getMap(std::string mapName) {
		auto iter = _loadedMaps.find(mapName);

		if (iter == _loadedMaps.end()) {
			warn("Map '", mapName, "' not found!");

			return Map{};
		}
		else {
			return iter->second;
		}
	}

	Camera* World::camera() {
		return &_camera;
	}

	int World::loadMap_bind(LuaState& lua) {

		auto map = Map{};

		map.setName(lua["name"]);

		map.setTmxFilePath(lua["path"]);

		single<World>().addMap(map);


		return 0;
	}

} //end namespace core