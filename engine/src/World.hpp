#ifndef CORE_WORLD_HPP
#define CORE_WORLD_HPP

#include <map>
#include "Templates.hpp"
#include "Map.hpp"
#include "LuaMap.hpp"
#include "LuaState.hpp"
#include "LuaTable.hpp"
#include "LuaVector.hpp"
#include "Camera.hpp"
#include "Tmx.h"
#include "Console.hpp"


namespace core {


	struct TileInformation : public LuaTable {

		TileInformation() {

			lua_reg("position", &position);
			lua_reg("dimensions", &dimensions);
			lua_reg("sourceTexturePosition", &sourceTexturePosition);
			lua_reg("textureName", &textureName);
			lua_reg("tileProperties", &tileProperties);
		}

		LuaPixel position;
		LuaDimension dimensions;
		LuaPixel sourceTexturePosition;
		std::string textureName;
		LuaMap tileProperties;
	};

	class World : public singleton<World>, public initializable<World, void, void, void, void> {

	public:

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		template<typename T>
		T getMapProperty(std::string key) {

		}

		template<>
		int getMapProperty(std::string key) {
			if (!_tmxMap.GetProperties().HasProperty(key)) {
				error("Tmx map has no property named '", key, "'.");
				return 0;
			}

			return _tmxMap.GetProperties().GetNumericProperty(key);
		}

		template<>
		float getMapProperty(std::string key) {
			if (!_tmxMap.GetProperties().HasProperty(key)) {
				error("Tmx map has no property named '", key, "'.");
				return 0;
			}

			return _tmxMap.GetProperties().GetFloatProperty(key);
		}

		template<>
		std::string getMapProperty(std::string key) {
			if (!_tmxMap.GetProperties().HasProperty(key)) {
				error("Tmx map has no property named '", key, "'.");
				return 0;
			}

			return _tmxMap.GetProperties().GetLiteralProperty(key);
		}

		template<typename T>
		T getLayerProperty(int layerId, std::string key) {

		}

		template<>
		int getLayerProperty(int layerId, std::string key) {
			auto tmxLayer = _tmxMap.GetLayer(layerId);
			if (tmxLayer == nullptr) {
				error("Attempting to get layer property of invalid layer (id ", layerId, ")");
				return 0;
			}

			if (!tmxLayer->GetProperties().HasProperty(key)) {
				error("Tmx layer ", layerId, " has no property named '", key, "'.");
				return 0;
			}

			return tmxLayer->GetProperties().GetNumericProperty(key);
		}

		template<>
		float getLayerProperty(int layerId, std::string key) {
			auto tmxLayer = _tmxMap.GetLayer(layerId);
			if (tmxLayer == nullptr) {
				error("Attempting to get layer property of invalid layer (id ", layerId, ")");
				return 0.0f;
			}

			if (!tmxLayer->GetProperties().HasProperty(key)) {
				error("Tmx layer ", layerId, " has no property named '", key, "'.");
				return 0.0f;
			}

			return tmxLayer->GetProperties().GetFloatProperty(key);
		}

		template<>
		std::string getLayerProperty(int layerId, std::string key) {
			auto tmxLayer = _tmxMap.GetLayer(layerId);
			if (tmxLayer == nullptr) {
				error("Attempting to get layer property of invalid layer (id ", layerId, ")");
				return "";
			}

			if (!tmxLayer->GetProperties().HasProperty(key)) {
				error("Tmx layer ", layerId, " has no property named '", key, "'.");
				return "";
			}

			return tmxLayer->GetProperties().GetLiteralProperty(key);

		}

		void runMap(std::string mapName, LuaFunction tileParseCallback);

		void addMap(Map map);

		Map getMap(std::string mapName);


		static int centerCameraAt_bind(LuaState& lua);

		static int cameraRoll_bind(LuaState& lua);
		
		static int cameraPitch_bind(LuaState& lua);

		static int cameraYaw_bind(LuaState& lua);

		static int cameraMove_bind(LuaState& lua);

		static int cameraZoom_bind(LuaState& lua);

		static int runMap_bind(LuaState& lua);

		static int loadMap_bind(LuaState& lua);
		
		Camera* camera();

	private:

		Camera _camera;

		std::map<std::string, Map> _maps;

		Tmx::Map _tmxMap;


	};


} //end namespace core

#endif