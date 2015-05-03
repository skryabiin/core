#include "Core.hpp"
#include "World.hpp"
#include "RenderableSystem2d.hpp"
#include "UpdateableSystem.hpp"
#include "AnimationSystem2d.hpp"
#include "TextureRenderSystem2d.hpp"
#include "TextRenderSystem2d.hpp"
#include "LinearParticleField.hpp"
#include "ParticleSystem2d.hpp"
#include "Interface.hpp"
#include "BasicPositionSystem2d.hpp"
#include "ShaderManager.hpp"

#include <stdlib.h>
#include <crtdbg.h>

namespace core {



	Core::Core() : _nextEntityId{ 0 }, _returnString{ "" } {
		


		_lua.create(true);
		_lua.initialize();
		_lua.bindFunction("create_bind", Core::create_bind);
		_lua.bindFunction("setDebug_bind", Console::setDebug_bind);
		_lua.bindFunction("createEntity_bind", Core::createEntity_bind);
		_lua.bindFunction("doQuit_bind", Core::doQuit_bind);
	}

	LuaState& Core::lua() {
		return _lua;
	}

	bool Core::createImpl() {
		info("Initializing Core...");

		if (single<EventProcessor>().create() == InitStatus::CREATE_FAILED) return false;

		if (single<Console>().create() == InitStatus::CREATE_FAILED) return false;

		if (single<Interface>().create() == InitStatus::CREATE_FAILED) return false;

		if (single<ResourceManager>().create() == InitStatus::CREATE_FAILED) return false;

		if (single<Renderer>().create() == InitStatus::CREATE_FAILED) return false;

		if (single<ShaderManager>().create() == InitStatus::CREATE_FAILED) return false;

		//this doens't belong here
		//lua_register(_L, "openMap_bind", World::openMap_bind);

		_lua.bindFunction("run_bind", Core::run_bind);
		_lua.bindFunction("init_bind", Core::init_bind);		
		_lua.bindFunction("destroyEntity_bind", Core::destroyEntity_bind);
		_lua.bindFunction("createSystem_bind", Core::createSystem_bind);
		_lua.bindFunction("reset_bind", Core::reset_bind);
		_lua.bindFunction("destroy_bind", Core::destroy_bind);

		_coreConfig.maxUpdatesPerSecond = _lua("PerformanceConfig")["maxUpdatesPerSecond"];

		return true;
	}

	bool Core::initializeImpl() {

		info("Initializing framework for scene...");

		if (single<EventProcessor>().initialize() == InitStatus::INIT_FAILED) {
			error("Event processor initialization failed.");
			return false;
		}

		if (single<Console>().initialize() == InitStatus::INIT_FAILED)  {
			error("Console initialization failed.");
			return false;
		}



		if (single<ResourceManager>().initialize() == InitStatus::INIT_FAILED)  {
			error("Resource manager initialization failed.");
			return false;
		}

		if (single<ShaderManager>().initialize() == InitStatus::INIT_FAILED)  {
			error("Shader manager initialization failed.");
			return false;
		}

		if (single<Renderer>().initialize() == InitStatus::INIT_FAILED)  {
			error("Renderer initialization failed.");
			return false;
		}
		if (single<Interface>().initialize() == InitStatus::INIT_FAILED)  {
			error("Interface initialization failed.");
			return false;
		}

		return true;
	}

	

	int Core::run() {

		auto multithreaded = single<Renderer>().isMultithreaded();

		_lastTick = SDL_GetTicks();
		
		auto fpsCounter = _lastTick;

		bool tickMinimumMet = false;
		

		_gogogo = true;		

		single<Renderer>().showWindow();


		while (_gogogo) {

			tickMinimumMet = resetRuntimeContext();

			if (!tickMinimumMet) {
				continue;
			}

			
			single<Interface>().update(_runtimeContext);

			update();		

			if (!multithreaded) {
				single<Renderer>().render();
			}
			

		}

		_lua.pushStack(_returnString);

		return 1;
	}

	bool Core::resetRuntimeContext()
	{

		//get the ticks from last time
		Uint32 ticks = SDL_GetTicks();

		int diff = ticks - _lastTick;

		if (diff < 1000 / _coreConfig.maxUpdatesPerSecond) {
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


	bool Core::handleEvent(WrappedSdlEvent& event)
	{		
		if (event._wrappedEvent->type == SDL_QUIT) {
			_returnString = "AppClosed";
			_gogogo = false;
		}
		return false;
	}

	void Core::pauseImpl() {

	}

	void Core::resumeImpl() {

	}


	void Core::update() {

		if (_paused) return;

		for (auto system : _updateableSystems) {
			if (!system->isPaused()) {
				system->update(_runtimeContext);
			}
		}

		_lua.call(_lua("Core")["update"], _runtimeContext.dt);


		single<Renderer>().update(_runtimeContext);
	}

	void Core::render() {



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

	void Core::doHardQuit(std::string msg) {
		std::string windowTitle = _lua("Config")["title"];
		auto title = "Fatal Error in " + windowTitle;
		single<Renderer>().hideWindow();
		single<Renderer>().pause();
		single<Core>().pause();
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), msg.c_str(), NULL);
		single<Core>().doQuit("Fatal");
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

		for (auto system : _systems) {
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

	
	System* Core::addSystem(System* system) {
		_systems.push_back(system);
		return _systems.back();
	}

	bool Core::resetImpl() {
		for (auto system : _systems) {
			system->reset();
			system->destroy();

		}
		info("Resetting renderer.");
		single<Renderer>().reset();

		_renderableSystems2d.clear();
		_updateableSystems.clear();

		for (auto system : _systems) {
			delete system;
		}		
		_systems.clear();

		single<Interface>().reset();

		single<ResourceManager>().reset();

		//single<ShaderManager>().reset();

		single<Console>().reset();

		single<EventProcessor>().reset();

		info("Core system reset complete.");
		return true;
	}

	bool Core::destroyImpl() {
		notice("Stopping Core at ", getTimestamp());
		notice("Reason: ", _returnString.c_str());

		single<Renderer>().destroy();
		single<Interface>().destroy();

		single<ResourceManager>().destroy();
		single<ShaderManager>().destroy();


		info("Core system destroy complete.");
		single<Console>().destroy();
		single<EventProcessor>().destroy();
		SDL_Quit();
		return true;
	}



	int Core::run_bind(LuaState& lua)  {
		return single<Core>().run();		
		return 1;
	}



	int Core::create_bind(LuaState& lua) {
		auto out = single<Core>().create() == InitStatus::CREATE_TRUE;
		lua.pushStack(out);
		return 1;
	}

	int Core::init_bind(LuaState& lua) {

		auto out = single<Core>().initialize() == InitStatus::INIT_TRUE;
		lua.pushStack(out);
		return 1;
	}


	int Core::reset_bind(LuaState& lua) {

		single<Core>().reset();

		return 0;
	}
	
	int Core::destroy_bind(LuaState& lua) {
		single<Core>().destroy();

		return 0;
	}


	int Core::createSystem_bind(LuaState& lua) {		
		std::string systemType = lua.pullStack<std::string>(1);
		std::string systemName = lua.pullStack<std::string>(2);

		if (!systemType.compare("WorldSystem")) {
			auto world = new World{};
			world->setName(systemName);
			single<Core>().addSystem(world);	
			world->create();
			world->initialize();
			lua.bindFunction("openMap_bind", World::openMap_bind);
			lua.pushStackPointer(world);
			return 1;
		}
		else if (!systemType.compare("BasicPositionSystem2d")) {
			auto positions = new BasicPositionSystem2d{};
			positions->setName(systemName);
			single<Core>().addSystem(positions);
			positions->create();
			positions->initialize();
			lua.bindFunction("addPositionFacet_bind", BasicPositionSystem2d::createFacet_bind);
		}

		else if (!systemType.compare("PhysicsSystem")) {
			auto physics = new PhysicsSystem{};
			physics->setName(systemName);
			single<Core>().addSystem(physics);
			single<Core>().includeUpdateableSystem(physics);
			physics->create();
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
			single<Core>().addSystem(animations);
			single<Core>().includeUpdateableSystem(animations);
			single<Core>().includeRenderableSystem2d(animations);
			animations->create();
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
			single<Core>().addSystem(textures);
			single<Core>().includeRenderableSystem2d(textures);
			textures->create();
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
			single<Core>().addSystem(texts);
			single<Core>().includeRenderableSystem2d(texts);
			texts->create();
			texts->initialize();
			lua.bindFunction("addTextFacet_bind", TextRenderSystem2d::createFacet_bind);
		}
		else if (!systemType.compare("ParticleSystem2d")) {
			
			auto particles = new ParticleSystem2d{};
			auto drawableLayerId = lua.pullStack<int>(3);
			particles->setDrawableLayerId(drawableLayerId);
			particles->setName(systemName);
			single<Core>().addSystem(particles);
			single<Core>().includeUpdateableSystem(particles);
			single<Core>().includeRenderableSystem2d(particles);
			particles->create();
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





	

} //end namespace core