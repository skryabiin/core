#ifndef CORE_CORE_H
#define CORE_CORE_H

#include <string>
#include "lua.hpp"

#include "System.hpp"
#include "LuaState.hpp"
#include "Util.hpp"
#include "Templates.hpp"
#include "Config.hpp"
#include "Event.hpp"
#include "ResourceManager.hpp"
#include "RuntimeContext.hpp"
#include "Entity.hpp"

#include "Console.hpp"
#include "Renderer.hpp"


#include "KeyboardEvent.hpp"
#include "GamepadEvent.hpp"



namespace core {

#define DEBUG_CORE	

	struct CoreConfig {
		int keyScancodePause;
		int buttonPause;
		int maxUpdatesPerSecond;
	};

	class RenderableSystem2d;
	class UpdateableSystem;

	class Core : public initializable<Core, void, void, void, void>, public singleton<Core>, public EventListener<WrappedSdlEvent>, public pausable<Core> {


	public:

		Core();

		bool createImpl();

		bool initializeImpl();

		bool resetImpl();

		bool destroyImpl();

		int run();

		bool resetRuntimeContext();

		bool handleEvent(WrappedSdlEvent& event);

		void update();
		
		void render();

		void updateMouseState();	

		void shutdown();

		void pauseImpl();

		void resumeImpl();

		void doQuit(std::string msg);

		void doHardQuit(std::string msg);
		
		template <typename Facet_type>
		std::vector<Facet_type*> getFacetsByType(Entity e) {

			auto facets = std::vector<Facet_type*>{};

			for (auto system : _systems) {
				auto results = system->getFacetsByType<Facet_type>(e);
				facets.insert(std::end(facets), std::begin(results), std::end(results));
			}

			return facets;

		}


		template <typename System_type>
		System_type* getSystemByName(std::string systemName) {


			for (auto system : _systems) {
				if (!system->name().compare(systemName)) {
					return dynamic_cast<System_type*>(system);
				}
			}

			return nullptr;

		}

		//lua bindings				

		static int run_bind(LuaState& lua);		

		static int doQuit_bind(LuaState& lua);

		

		static int create_bind(LuaState& lua);

		static int init_bind(LuaState& lua);

		static int reset_bind(LuaState& lua);

		static int destroy_bind(LuaState& lua);

		static int createSystem_bind(LuaState& lua);

		static int createEntity_bind(LuaState& lua);

		static int destroyEntity_bind(LuaState& lua);

		static int scheduleTimedEvent_bind(LuaState& lua);


		//entity methods
		Entity createEntity();

		void destroyEntity(Entity& entity);


		//system methods
		UpdateableSystem* includeUpdateableSystem(UpdateableSystem* system);

		RenderableSystem2d* includeRenderableSystem2d(RenderableSystem2d* system);

		System* addSystem(System* system);
		
		void cleanup();


		//lua methods
		bool runScript(std::string scriptPath);



		LuaState& lua();

	private:

		

		LuaState _lua;

		//local config object
		CoreConfig _coreConfig;

	

		//lua return string
		std::string _returnString;


		//flag for whether to continue the main loop
		bool _gogogo;		

		//the runtime context that gets passed to update methods
		RuntimeContext _runtimeContext;


		//the last ms tick
		int _lastTick;


		//entity list
		std::vector<Entity> _entities;

		//the next id to be assigned (will be incremented before assignment)
		int _nextEntityId;

		

		//system lists
		//these systems are called each round with the runtime context
		std::vector<UpdateableSystem*> _updateableSystems;


		//these systems are called during the render phase
		std::vector<RenderableSystem2d*> _renderableSystems2d;


		//these systems interact in other ways
		std::vector<System*> _systems;

		struct TimedEventCallback {


		};


		static std::map<std::string, std::function<int(lua_State*)>> registeredBindings();

	};



} //end namespace core


#endif