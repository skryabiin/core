#ifndef CORE_CORE_H
#define CORE_CORE_H

#include <string>
#include "lua.hpp"

#include "LuaState.hpp"
#include "Util.hpp"
#include "Templates.hpp"
#include "Config.hpp"
#include "Event.hpp"
#include "ResourceManager.hpp"
#include "RuntimeContext.hpp"
#include "Entity.hpp"
#include "System.hpp"
#include "Console.hpp"
#include "Renderer.hpp"
#include "UpdateableSystem.hpp"
#include "RenderableSystem2d.hpp"
#include "KeyboardEvent.hpp"
#include "GamepadEvent.hpp"



namespace core {

#define DEBUG_CORE	

	struct CoreConfig {
		int keyScancodePause;
		int buttonPause;
		int maxFps;
	};

	class Core : public initializable<Core, void, void>, public singleton<Core>, public EventListener, public pausable<Core> {


	public:

		Core();

		InitStatus initializeImpl();

		InitStatus resetImpl();

		int run();

		bool resetRuntimeContext();

		void handleSdlEvent(WrappedSdlEvent& event);

		void handleKeyboardEvent(KeyboardEvent& event);

		void handleGamepadEvent(GamepadEvent& event);

		void update();
		
		void render();

		void updateMouseState();	

		void shutdown();

		void pauseImpl();

		void resumeImpl();

		
		
		template <typename Facet_type>
		std::vector<Facet_type*> getFacetsByType(Entity e) {

			auto facets = std::vector<Facet_type*>{};

			for (auto& system : _systems) {
				auto results = system->getFacetsByType<Facet_type>(e);
				facets.insert(std::end(facets), std::begin(results), std::end(results));
			}

			return facets;

		}


		template <typename System_type>
		System_type* getSystemByName(std::string systemName) {


			for (auto& system : _systems) {
				if (!system->name().compare(systemName)) {
					return dynamic_cast<System_type*>(system.get());
				}
			}

			return nullptr;

		}

		//lua bindings				

		static int run_bind(LuaState& lua);		

		static int doQuit_bind(LuaState& lua);

		static int reset_bind(LuaState& lua);

		static int init_bind(LuaState& lua);

		static int shutdown_bind(LuaState& lua);

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

		System* addSystem(std::unique_ptr<System>&& system);
		
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


		//filter for certain sdl events like quit
		EventFilter<WrappedSdlEvent> _sdlEventFilter;


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
		std::vector<std::unique_ptr<System>> _systems;

		struct TimedEventCallback {


		};


		static std::map<std::string, std::function<int(lua_State*)>> registeredBindings();

	};



} //end namespace core


#endif