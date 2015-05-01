//
//  World.cpp
//  CollisionTests
//
//  Created by Jared Lynem on 10/28/13.
//
//
#include "lua.hpp"
#include "World.hpp"
#include "Geometry.hpp"
#include "Renderer.hpp"
#include "Util.hpp"
#include "Console.hpp"
#include "Templates.hpp"
#include "Core.hpp"
#include "Animation.hpp"
#include "AnimationSystem2d.hpp"
#include "Texture.hpp"

namespace core {

	bool World::createImpl() {
		return true;
	}

	bool World::initializeImpl() {

		return System::initializeImpl();
	}

	bool World::resetImpl() {
		_map.~Map();

		return System::resetImpl();
	}

	bool World::destroyImpl() {
		return true;
	}

	void World::openMap(std::string mapName,int callbackRef) {


		_map.ParseFile(single<ResourceManager>().getMap(mapName).tmxFilePath());
		

		//set the background color
		unsigned short r, g, b;
		auto c = Color{};
		if (!_map.GetBackgroundColor().compare("")) {
			r = 0x00;
			g = 0x00;
			b = 0x00;
		}
		else {
			hexToRgb(_map.GetBackgroundColor(), c.r, c.g, c.b);			
		}
		c.a = 0xFF;




		//load the tilesets into the resource manager
		const std::vector<Tmx::Tileset*> tilesets = _map.GetTilesets();
		for (auto ts : tilesets) {
			auto tsimage = ts->GetImage();
			auto texture = new Texture{};
			texture->setName(ts->GetName());
			
			texture->setFileSource("./assets/maps/tilesets/" + stripPath(tsimage->GetSource()));			
			texture->initialize();
			single<ResourceManager>().addTexture(std::unique_ptr<Texture>(texture));			
		}

		auto& lua = single<Core>().lua();
		const std::vector<Tmx::Layer*> layers = _map.GetLayers();
		for (auto layer : layers) {

			int x1, y1;
			int u1, v1;
			int layerWidth = layer->GetWidth();
			int layerHeight = layer->GetHeight();


			// so much shit code
			single<Core>().getSystemByName<PhysicsSystem>("Physics")->setGravity(Vec2{ layer->GetProperties().GetFloatProperty("defaultGravityX"), layer->GetProperties().GetFloatProperty("defaultGravityY") });
			for (int y = 0; y < layerHeight; y++) {
				for (int x = 0; x < layerWidth; x++) {

					int tileId = layer->GetTileId(x, layerHeight - y - 1);
					int tilesetId = layer->GetTileTilesetIndex(x, layerHeight - y - 1);
					
					if (tilesetId >= 0) {


						auto thisSet = _map.GetTileset(tilesetId);
						auto tile = _map.GetTileset(tilesetId)->GetTile(tileId);

						//figure out which section of the texture this tile is
						auto setw = (thisSet->GetImage()->GetWidth() / thisSet->GetTileWidth());
						auto seth = (thisSet->GetImage()->GetHeight() / thisSet->GetTileHeight());

						u1 = tileId % setw;
						v1 = (tileId - u1) / setw;
						u1 *= thisSet->GetTileWidth();
						v1 *= thisSet->GetTileHeight();

						x1 = x * _map.GetTileWidth();
						y1 = y * _map.GetTileHeight();


						auto luaTile = LuaTile{};
						

						luaTile.position[0] = x1;
						luaTile.position[1] = y1;
						luaTile.position[2] = 0;
						luaTile.dimensions[0] = thisSet->GetTileWidth();
						luaTile.dimensions[1] = thisSet->GetTileHeight();
						luaTile.sourceTextureRect[0] = u1;
						luaTile.sourceTextureRect[1] = v1;
						luaTile.sourceTextureRect[2] = thisSet->GetTileWidth();
						luaTile.sourceTextureRect[3] = thisSet->GetTileHeight();
						luaTile.textureName = thisSet->GetName();
						auto& tileProperties = tile->GetProperties();
						luaTile.tileProperties = tileProperties.GetList();
						lua.call(callbackRef, luaTile);

					}

				} //end layer X loop

			} //end layer Y loop

		} //end layers loop

		for (auto objGroup : _map.GetObjectGroups()) {


			//get the associated tile layer
			auto tileLayer = objGroup->GetProperties().GetLiteralProperty("tileLayer");

			auto physicsSystem = single<Core>().getSystemByName<PhysicsSystem>("Physics");

			for (auto obj : objGroup->GetObjects()) {


				debug("Object at ", obj->GetX(), ", ", obj->GetY());
				auto luaTile = LuaTile{};
				luaTile.position[0] = obj->GetX();
				luaTile.position[1] = _map.GetTileHeight() * objGroup->GetHeight() - obj->GetY() - obj->GetHeight();
				luaTile.position[2] = 0;
				luaTile.dimensions[0] = obj->GetWidth();
				luaTile.dimensions[1] = obj->GetHeight();
				luaTile.isObject = true;
				luaTile.tileProperties = obj->GetProperties().GetList();
				luaTile.tileProperties["name"] = obj->GetName();
				luaTile.tileProperties["objectType"] = obj->GetType();
				
				lua.call(callbackRef, luaTile);

			} //end objects loop

		} //end object layers loop

		lua.removeReference(callbackRef);
	}



	int World::openMap_bind(LuaState& lua) {

		auto worldSystemName = lua.pullStack<std::string>(1);

		auto worldSystem = single<Core>().getSystemByName<World>(worldSystemName);
		
		if (worldSystem == nullptr) {

			error("World system '", worldSystemName, "' does not exist!");
				
			return 0;
		}
		auto mapName = lua.pullStack<std::string>(2);
		auto callbackRef = lua.createReference();

		worldSystem->openMap(mapName, callbackRef);

		

		return 0;
	}


} //end namespace core
