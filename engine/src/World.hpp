#ifndef CORE_WORLD_HPP
#define CORE_WORLD_HPP

#include <map>
#include "Templates.hpp"
#include "Map.hpp"
#include "LuaState.hpp"
#include "Camera.hpp"

namespace core {


	class World : public singleton<World>, public initializable<World, void, void, void, void> {

	public:

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		void addMap(Map map);

		Map getMap(std::string mapName);


		static int loadMap_bind(LuaState& lua);

		Camera* camera();

	private:


		Camera _camera;

		std::map<std::string, Map> _loadedMaps;



	};


} //end namespace core

#endif