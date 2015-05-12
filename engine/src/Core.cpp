#include "Core.hpp"
#include "World.hpp"
#include "RenderableSystem2d.hpp"
#include "UpdateableSystem.hpp"
#include "AnimationSystem2d.hpp"
#include "TextureRenderSystem2d.hpp"
#include "TextRenderSystem2d.hpp"
#include "BasicAudioSystem.hpp"
#include "LinearParticleField.hpp"
#include "ParticleSystem2d.hpp"
#include "Interface.hpp"
#include "BasicPositionSystem2d.hpp"
#include "ShaderManager.hpp"
#include "TransitionManager.hpp"
#include "AudioManager.hpp"
#include "PhysicsSystem.hpp"

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
		notice("---------------------------------------------");
		notice("Initializing Core at ", getTimestamp());

		_coreConfig.debugMemory = _lua("Config")["system"]["debugMemory"];
		if (_coreConfig.debugMemory) {
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		}

		if (single<EventProcessor>().create() == InitStatus::CREATE_FAILED) {
			error("Event processor creation failed.");
			return false;
		}

		if (single<Console>().create() == InitStatus::CREATE_FAILED) {
			error("Event processor creation failed.");
			return false;
		}

		if (single<Interface>().create() == InitStatus::CREATE_FAILED) {
			error("Interface creation failed.");
			return false;
		}

		if (single<TextureManager>().create() == InitStatus::CREATE_FAILED) {
			error("Resource manager creation failed.");
			return false;
		}

		if (single<AudioManager>().create() == InitStatus::CREATE_FAILED) {
			error("Audio manager creation failed.");
			return false;
		}

		if (single<Renderer>().create() == InitStatus::CREATE_FAILED) {
			error("Renderer creation failed.");
			return false;
		}

		if (single<ShaderManager>().create() == InitStatus::CREATE_FAILED) {
			error("Shader manager creation failed.");
			return false;
		}

		if (single<TransitionManager>().create() == InitStatus::CREATE_FAILED) {
			error("Transition manager creation failed.");
			return false;
		}

		if (single<World>().create() == InitStatus::CREATE_FAILED) {
			error("World creation failed.");
		}
		

		//this doens't belong here
		//lua_register(_L, "openMap_bind", World::openMap_bind);

		_lua.bindFunction("run_bind", Core::run_bind);
		_lua.bindFunction("init_bind", Core::init_bind);		
		_lua.bindFunction("destroyEntity_bind", Core::destroyEntity_bind);
		_lua.bindFunction("createSystem_bind", Core::createSystem_bind);
		_lua.bindFunction("reset_bind", Core::reset_bind);
		_lua.bindFunction("destroy_bind", Core::destroy_bind);

		_coreConfig.maxUpdatesPerSecond = _lua("Config")["system"]["maxUpdatesPerSecond"];

		_luaUpdateFunction = _lua("Core")["update"];

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

		if (single<TextureManager>().initialize() == InitStatus::INIT_FAILED)  {
			error("Resource manager initialization failed.");
			return false;
		}

		if (single<AudioManager>().initialize() == InitStatus::INIT_FAILED)  {
			error("Audio manager initialization failed.");
			return false;
		}

		if (single<ShaderManager>().initialize() == InitStatus::INIT_FAILED)  {
			error("Shader manager initialization failed.");
			return false;
		}

		if (single<TransitionManager>().initialize() == InitStatus::INIT_FAILED)  {
			error("Transition manager initialization failed.");
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

		if (single<World>().initialize() == InitStatus::INIT_FAILED) {
			error("World initialization failed.");
		}

		return true;
	}

	

	int Core::run() {

		_frames = 0;
		_elapsed = 0;

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
			SDL_Delay(1);
			return false;
		}
		
		//TODO make this an actually useful feature
		/*
		_frames++;
		_elapsed += diff;
		*/
		_runtimeContext.dt = ticks - _lastTick;

		/*
		if (_frames % 100 == 0) {
			float fps = (_frames * 1000) / (_elapsed);
			debug("diff: ", diff);
		}
		*/
#ifdef DEBUG_CORE
		//a sane update duration in case we're debugging
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
		
		auto floatdt = (float)_runtimeContext.dt;

		single<Interface>().update(floatdt, _runtimeContext);

		if (_paused) return;

		single<TransitionManager>().update(floatdt, _runtimeContext);

		for (auto system : _updateableSystems) {
			if (!system->isPaused()) {
				system->update(floatdt, _runtimeContext);
			}
		}

		_lua.call(_luaUpdateFunction, _runtimeContext.dt);


		single<Renderer>().update(floatdt, _runtimeContext);

		single<World>().camera()->roll(floatdt * 20.0f / 1000.0f);
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

	long Core::getSystemIdByName(std::string systemName) {

		for (auto system : _systems) {
			if (!system->name().compare(systemName)) {
				return system->id();
			}
		}
		return -1;
	}
	std::string Core::getSystemNameById(long id) {
		for (auto system : _systems) {
			if (system->id() == id) {
				return system->name();
			}
		}
		return "Not found";
	}
	void Core::doQuit(std::string msg) {
		single<Core>()._gogogo = false;
		single<Core>()._returnString = msg;
	}

	void Core::doHardQuit(std::string msg) {
		std::string windowTitle = _lua("Config")["window"]["title"];
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


		}		
		single<Renderer>().reset();

		_renderableSystems2d.clear();
		_updateableSystems.clear();

		for (auto system : _systems) {
			system->destroy();
			delete system;
		}		
		_systems.clear();
		

		single<TransitionManager>().reset();

		single<Interface>().reset();

		single<TextureManager>().reset();

		single<AudioManager>().reset();

		//single<ShaderManager>().reset();

		single<Console>().reset();

		single<EventProcessor>().reset();

		info("Core system reset complete.");
		return true;
	}

	bool Core::destroyImpl() {
		notice("Stopping Core with message: ", _returnString.c_str());		


		single<TransitionManager>().destroy();

		single<Renderer>().destroy();
		single<Interface>().destroy();

		single<TextureManager>().destroy();
		single<AudioManager>().destroy();
		single<ShaderManager>().destroy();
		


		notice("Core system destroy complete at ", getTimestamp());
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

		
		if (!systemType.compare("BasicPositionSystem2d")) {
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
		else if (!systemType.compare("BasicAudioSystem")) {
			auto audio = new BasicAudioSystem{};
			audio->setName(systemName);
			single<Core>().addSystem(audio);
			audio->create();
			audio->initialize();
			lua.bindFunction("addAudioFacet_bind", BasicAudioSystem::createFacet_bind);
			lua.bindFunction("updateAudioFacet_bind", BasicAudioSystem::updateFacet_bind);
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