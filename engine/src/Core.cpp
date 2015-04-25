#include "Core.hpp"
#include "World.hpp"
#include "AnimationSystem2d.hpp"
#include "TextureRenderSystem2d.hpp"
#include "TextRenderSystem2d.hpp"
#include "LinearParticleField.hpp"
#include "ParticleSystem2d.hpp"
#include "Interface.hpp"
#include "BasicPositionSystem2d.hpp"
#include "ShaderManager.hpp"

namespace core {



	Core::Core() : _nextEntityId{ 0 }, _returnString{ "" } {
		


		_lua.initialize(true);

		_lua.bindFunction("init_bind", Core::init_bind);
		_lua.bindFunction("setDebug_bind", Console::setDebug_bind);
		_lua.bindFunction("createEntity_bind", Core::createEntity_bind);
		_lua.bindFunction("doQuit_bind", Core::doQuit_bind);
	}

	LuaState& Core::lua() {
		return _lua;
	}



	InitStatus Core::initializeImpl() {


		info("Initializing Core...");

		if (single<EventProcessor>().initialize() == InitStatus::INIT_FAILED) return InitStatus::INIT_FAILED;

		if (single<Console>().initialize() == InitStatus::INIT_FAILED) return InitStatus::INIT_FAILED;

		if (single<Interface>().initialize() == InitStatus::INIT_FAILED) return InitStatus::INIT_FAILED;

		if (single<ResourceManager>().initialize() == InitStatus::INIT_FAILED) return InitStatus::INIT_FAILED;

		if (single<Renderer>().initialize() == InitStatus::INIT_FAILED) return InitStatus::INIT_FAILED;

		if (single<ShaderManager>().initialize() == InitStatus::INIT_FAILED) return InitStatus::INIT_FAILED;

		//this doens't belong here
		//lua_register(_L, "openMap_bind", World::openMap_bind);

		_lua.bindFunction("run_bind", Core::run_bind);
		_lua.bindFunction("createEntity_bind", Core::createEntity_bind);
		_lua.bindFunction("destroyEntity_bind", Core::destroyEntity_bind);
		_lua.bindFunction("createSystem_bind", Core::createSystem_bind);
		_lua.bindFunction("reset_bind", Core::reset_bind);
		_lua.bindFunction("shutdown_bind", Core::shutdown_bind);


		_coreConfig.buttonPause = _lua("Config")["buttonPause"];
		_coreConfig.keyScancodePause = _lua("Config")["keyScancodePause"];
		_coreConfig.maxFps = _lua("Config")["maxFps"];


			
		//create the callback for keyboard events
		std::function<void(Core*, WrappedSdlEvent&)> sdlEventCallback = std::mem_fn(&Core::handleSdlEvent);

		//now set up the filter
		_sdlEventFilter.init(this, sdlEventCallback);
		single<EventProcessor>().addFilter(&_sdlEventFilter);


		

		return InitStatus::INIT_TRUE;
	}

	

	int Core::run() {


		_lastTick = SDL_GetTicks();
		
		auto fpsCounter = _lastTick;

		bool tickMinimumMet = false;
		
		auto& eventProcessor = single<EventProcessor>();
		auto& renderer = single<Renderer>();

		single<Interface>().initialize();


		single<Renderer>().resume();

		_gogogo = true;		


		while (_gogogo) {

			tickMinimumMet = resetRuntimeContext();

			if (!tickMinimumMet) {
				continue;
			}

			
			single<Interface>().update(_runtimeContext);

			update();

			//render();

		}

		single<Renderer>().pause();
		_lua.pushStack(_returnString);

		return 1;
	}

	bool Core::resetRuntimeContext()
	{

		//get the ticks from last time
		Uint32 ticks = SDL_GetTicks();

		int diff = ticks - _lastTick;

		if (diff < 1000 / _coreConfig.maxFps) {
			SDL_Delay(5);
			return false;
		}
		else {
			//console() << "Losing framerate. This update is " << diff << " ticks\n";
		}

		_runtimeContext.dt = ticks - _lastTick;

#ifdef DEBUG_CORE

		_runtimeContext.dt = (_runtimeContext.dt > 200) ? 17 : _runtimeContext.dt;
#endif

		_runtimeContext.dt0 = _runtimeContext.dt;
		_lastTick = ticks;


		return true;
	}


	void Core::handleSdlEvent(WrappedSdlEvent& event)
	{
		//debug("SDL Event: ", event._wrappedEvent->type);
		if (event._wrappedEvent->type == SDL_QUIT) {
			_returnString = "AppClosed";
			_gogogo = false;
		}
	}

	void Core::handleKeyboardEvent(KeyboardEvent& event) {

		if (event.scancode == _coreConfig.keyScancodePause) {}
		
	}

	void Core::handleGamepadEvent(GamepadEvent& event) {

		if (event.down && event.button == _coreConfig.buttonPause) {}
		
	}


	void Core::pauseImpl() {

	}

	void Core::resumeImpl() {

	}


	void Core::update() {

		if (_paused) return;

		for (auto& system : _updateableSystems) {
			if (!system->isPaused()) {
				system->update(_runtimeContext);
			}
		}

		_lua.call("Core.update", _runtimeContext.dt);
	}

	void Core::render() {


		single<Renderer>().render();

		/*
		single<Renderer>().start();

		for (auto& system : _renderableSystems2d) {
			if (!system->isPaused()) {
				system->render();
			}
		}

		single<Renderer>().end();
		*/
	}

	void Core::doQuit(std::string msg) {
		single<Core>()._gogogo = false;
		single<Core>()._returnString = msg;
	}


	int Core::doQuit_bind(LuaState& lua) {

		std::string returnMessage = lua.pullStack<std::string>(1);
		single<Core>().doQuit(returnMessage);
		return 0;
	}

	Entity Core::createEntity() {

		auto e = Entity{};
		e = ++_nextEntityId;
		_entities.push_back(e);
		return e;
	}

	void Core::destroyEntity(Entity& entity) {

		for (auto& system : _systems) {
			system->destroyFacets(entity);
		}

		//TODO need to set up a free entity list

	}

	UpdateableSystem* Core::includeUpdateableSystem(UpdateableSystem* system) {
		_updateableSystems.push_back(system);
		return _updateableSystems.back();
	}


	RenderableSystem2d* Core::includeRenderableSystem2d(RenderableSystem2d* system) {
		_renderableSystems2d.push_back(system);
		return _renderableSystems2d.back();
	}

	
	System* Core::addSystem(std::unique_ptr<System>&& system) {
		_systems.push_back(std::move(system));
		return _systems.back().get();
	}



	void Core::shutdown() {

		if (_initStatus != InitStatus::INIT_FALSE) {

			notice("Stopping Core at ", getTimestamp());
			notice("Reason: ", _returnString.c_str());

			
			cleanup();

			single<Console>().shutdown();
			single<Renderer>().cleanup();
			SDL_Quit();

			_initStatus = InitStatus::INIT_FALSE;
		}

	}

	void Core::cleanup() {		
		_renderableSystems2d.clear();
		_updateableSystems.clear();
		for (auto& system : _systems) {
			system->resetImpl();
		}
		_systems.clear();
		_systems.shrink_to_fit();
		
		single<Interface>().reset();
		single<EventProcessor>().reset();

		info("Core system cleanup complete.");
	}


	int Core::run_bind(LuaState& lua)  {
		return single<Core>().run();		
		return 1;
	}


	int Core::reset_bind(LuaState& lua) {
		
		single<Core>().cleanup();

		return 0;
	}

	int Core::init_bind(LuaState& lua) {

		auto out = single<Core>().initialize() == InitStatus::INIT_TRUE;
		lua.pushStack(out);
		return 1;
	}
	
	int Core::createSystem_bind(LuaState& lua) {		
		std::string systemType = lua.pullStack<std::string>(1);
		std::string systemName = lua.pullStack<std::string>(2);

		if (!systemType.compare("WorldSystem")) {
			auto world = new World{};
			world->setName(systemName);
			single<Core>().addSystem(std::unique_ptr<System>(world));	
			world->initialize();
			lua.bindFunction("openMap_bind", World::openMap_bind);
			lua.pushStackPointer(world);
			return 1;
		}
		else if (!systemType.compare("BasicPositionSystem2d")) {
			auto positions = new BasicPositionSystem2d{};
			positions->setName(systemName);
			single<Core>().addSystem(std::unique_ptr<System>(positions));
			positions->initialize();
			lua.bindFunction("addPositionFacet_bind", BasicPositionSystem2d::createFacet_bind);
		}

		else if (!systemType.compare("PhysicsSystem")) {
			auto physics = new PhysicsSystem{};
			physics->setName(systemName);
			single<Core>().addSystem(std::unique_ptr<System>(physics));
			single<Core>().includeUpdateableSystem(physics);
			physics->initialize();
			lua.bindFunction("addPhysicsFacet_bind", PhysicsSystem::createFacet_bind);
			lua.pushStackPointer(physics);
			return 1;
		}
		else if (!systemType.compare("AnimationSystem2d")) {
			
			auto animations = new AnimationSystem2d{};
			auto drawableLayerId = lua.pullStack<int>(3);
			animations->setDrawableLayerId(drawableLayerId);
			animations->setName(systemName);
			single<Core>().addSystem(std::unique_ptr<System>(animations));
			single<Core>().includeUpdateableSystem(animations);
			single<Core>().includeRenderableSystem2d(animations);
			animations->initialize();
			lua.bindFunction("addAnimationFacet_bind", AnimationSystem2d::createFacet_bind);
			lua.pushStackPointer(animations);
			return 1;
		}
		else if (!systemType.compare("TextureRenderSystem2d")) {

			auto textures = new TextureRenderSystem2d{};
			auto drawableLayerId = lua.pullStack<int>(3);
			textures->setDrawableLayerId(drawableLayerId);
			textures->setName(systemName);
			single<Core>().addSystem(std::unique_ptr<System>(textures));
			single<Core>().includeRenderableSystem2d(textures);
			textures->initialize();
			lua.bindFunction("addTextureFacet_bind", TextureRenderSystem2d::createFacet_bind);
			lua.bindFunction("addCameraFollowFacet_bind", RenderableSystem2d::createFacet_bind);
			lua.pushStackPointer(textures);
			return 1;
		} 
		else if (!systemType.compare("TextRenderSystem2d")) {			
			auto texts = new TextRenderSystem2d{};
			auto drawableLayerId = lua.pullStack<int>(3);
			texts->setDrawableLayerId(drawableLayerId);

			texts->setName(systemName);
			single<Core>().addSystem(std::unique_ptr<System>(texts));
			single<Core>().includeRenderableSystem2d(texts);
			texts->initialize();
			lua.bindFunction("addTextFacet_bind", TextRenderSystem2d::createFacet_bind);
		}
		else if (!systemType.compare("ParticleSystem2d")) {
			
			auto particles = new ParticleSystem2d{};
			auto drawableLayerId = lua.pullStack<int>(3);
			particles->setDrawableLayerId(drawableLayerId);
			particles->setName(systemName);
			single<Core>().addSystem(std::unique_ptr<System>(particles));
			single<Core>().includeUpdateableSystem(particles);
			single<Core>().includeRenderableSystem2d(particles);
			particles->initialize();
			lua.bindFunction("addParticleFacet_bind", ParticleSystem2d::createFacet_bind);
		}
		
		
		return 0;
	}

	int Core::createEntity_bind(LuaState& lua) {

		auto e = single<Core>().createEntity();
		lua.pushStack(e);
		return 1;
	}

	int Core::destroyEntity_bind(LuaState& lua) {
		auto entityId = lua.pullStack<int>(1);		
		single<Core>().destroyEntity(entityId);
		return 0;
	}



	int Core::shutdown_bind(LuaState& lua) {
		single<Core>().shutdown();

		return 0;
	}


	

} //end namespace core