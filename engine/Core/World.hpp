//
//  World.h
//  CollisionTests
//
//  Created by Jared Lynem on 10/28/13.
//
//

#ifndef __CollisionTests__World__
#define __CollisionTests__World__

#include <iostream>
#include <vector>
#include <memory>

#include "Tmx.h"

#include "Templates.hpp"
#include "System.hpp"
#include "PhysicsSystem.hpp"
#include "Event.hpp"
#include "Resource.hpp"
#include "LuaTable.hpp"
#include "LuaMap.hpp"

namespace core {




	struct LuaTile : public LuaTable {

		LuaTile() {
			lua_reg("position", &position);
			lua_reg("dimensions", &dimensions);
			lua_reg("sourceTextureRect", &sourceTextureRect);
			lua_reg("textureName", &textureName);
			lua_reg("tileProperties", &tileProperties);
			lua_reg("isObject", &isObject);
		}

		LuaPixel position;
		LuaDimension dimensions;
		LuaRect sourceTextureRect;
		std::string textureName;
		LuaMap tileProperties;
		bool isObject;
	};


	class World : public System {

	public:

		bool init();

		void openMap(std::string mapName, int callbackRef);


		virtual InitStatus initializeImpl() override;

		virtual InitStatus resetImpl() override;		
		
		static int openMap_bind(LuaState& lua);
		
		virtual void destroyFacets(Entity& entity) override {};

		virtual void handleFacetPauseEvent(FacetPauseEvent& pauseEvent) override {};

	private:

		void doLuaTileCallback(LuaState& lua, LuaTile& t, int callbackRef);


		Tmx::Map _map;



	};


} //end namespace core

#endif /* defined(__CollisionTests__World__) */
