#include <SDL.h>
#include "Tmx.h"
#include "ResourceManager.hpp"
#include "Renderer.hpp"
#include "Util.hpp"
#include "SDLUtil.hpp"
#include "Console.hpp"
#include "Core.hpp"
#include "Font.hpp"


namespace core {



	bool ResourceManager::createImpl() {

		info("Creating ResourceManager:");

		//init .png loading
		info("Initializing SDL_image...");
		int imgFlags = IMG_INIT_PNG;
		if (!IMG_Init(imgFlags)) {
			error("Error initializing SDL_image: ", IMG_GetError());
			return false;
		}

		//init tt font loading

		info("Initializing SDL_ttf...");

		if (TTF_Init() == -1) {
			error("Error initializing SDL_ttf: ", TTF_GetError());
			return false;
		}

		info("Initializing SDL_audio...");

		if (SDL_Init(SDL_INIT_AUDIO) < 0)  {
			error("Error initializing SDL audio: ", SDL_GetError());
			return false;
		}

		info("Initializing SDL_mixer...");

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
			error("Error initializing SDL_mixer: ", Mix_GetError());
			return false;
		}

		info("Registering lua functions...");

		//register lua functions
		auto& lua = single<Core>().lua();
		lua.bindFunction("loadFont_bind", loadFont_bind);
		lua.bindFunction("loadSound_bind", loadSound_bind);
		lua.bindFunction("loadTexture_bind", loadTexture_bind);
		lua.bindFunction("loadAnimationSet_bind", loadAnimationSet_bind);
		lua.bindFunction("loadMap_bind", loadMap_bind);
		lua.bindFunction("playSound_bind", playSound_bind);
		lua.bindFunction("loadParticleEffect_bind", loadParticleEffect_bind);

		return true;
	}

	bool ResourceManager::initializeImpl() {
		for (auto& texture : _loadedTextures) {
			texture.second.get()->initialize();
		}

		for (auto& font : _loadedFonts) {
			font.second.get()->initialize();
		}


		for (auto& sound : _loadedSounds) {
			sound.second.get()->initialize();
		}

		for (auto& music : _loadedMusic) {
			music.second.get()->initialize();
		}

		for (auto& map : _loadedMaps) {
			map.second.initialize();
		}
		return true;
	}

	bool ResourceManager::resetImpl() {
		
		for (auto& texture : _loadedTextures) {
			texture.second.get()->reset();
		}

		for (auto& font : _loadedFonts) {
			font.second.get()->reset();
		}


		for (auto& sound : _loadedSounds) {
			sound.second.get()->reset();
		}
		
		for (auto& music : _loadedMusic) {
			music.second.get()->reset();
		}
		
		for (auto& map : _loadedMaps) {
			map.second.reset();
		}
		
		//TODO reset everything
		return true;
	}

	bool ResourceManager::destroyImpl() {

		for (auto& texture : _loadedTextures) {
			texture.second.get()->destroy();
		}
		_loadedTextures.clear();

		_loadedAnimationSets.clear();

		for (auto& font : _loadedFonts) {
			font.second.get()->destroy();
		}
		_loadedFonts.clear();

		for (auto& sound : _loadedSounds) {
			sound.second.get()->destroy();
		}

		_loadedSounds.clear();

		for (auto& music : _loadedMusic) {
			music.second.get()->destroy();
		}
		_loadedMusic.clear();

		for (auto& map : _loadedMaps) {
			map.second.destroy();
		}

		_loadedMaps.clear();

		//TODO why aren't these resources
		_loadedParticleEffects.clear();
		return true;
	}

	Font* ResourceManager::addFont(std::unique_ptr<Font> font) {
		std::string name = font->name();
		_loadedFonts.insert(std::pair<std::string, std::unique_ptr<Font>>(font->name(), std::move(font)));
		return _loadedFonts.find(name)->second.get();
	}

	Font* ResourceManager::getFont(std::string name) {
		
		auto iter = _loadedFonts.find(name);

		if (iter == _loadedFonts.end()) {
			warn("Font '", name,"' not found!");

			return nullptr;
		}
		else {
			return iter->second.get();
		}
	}


	Font* ResourceManager::getDefaultFont() {
		return _defaultFont;
	}

	void ResourceManager::setDefaultFont(std::string fontName) {

		for (auto& font : _loadedFonts) {

			if (!font.second->name().compare(fontName)) {
				_defaultFont = font.second.get();
				break;
			}
		}
	}


	Texture* ResourceManager::addTexture(std::unique_ptr<Texture> texture) {
		std::string name = texture->name();
		_loadedTextures.insert(std::pair<std::string, std::unique_ptr<Texture>>(texture->name(), std::move(texture)));
		return getTexture(name);
	}

	void ResourceManager::removeTexture(std::string name) {
		_loadedTextures.erase(name);
	}

	Texture* ResourceManager::getTexture(std::string name) {

		std::map<std::string, std::unique_ptr<Texture>>::iterator iter = _loadedTextures.find(name);

		if (iter == _loadedTextures.end()) {
			warn("Texture '", name, "' not found!");

			return nullptr;
		} else {
			return iter->second.get();
		}

	}

	void ResourceManager::addAnimationSet(AnimationSet animationSet) {
		_loadedAnimationSets.insert(std::pair<std::string, AnimationSet>(animationSet.name(), animationSet));
	}

	AnimationSet ResourceManager::getAnimationSet(std::string name) const {

		auto& it = _loadedAnimationSets.find(name);
		if (it == end(_loadedAnimationSets)) {
			warn("AnimationSet '", name, "' not found!");			
			return AnimationSet{};
		}
		else {
			return it->second;
		}

	}


	void ResourceManager::addSound(std::unique_ptr<Sound> sound) {
		_loadedSounds.insert(std::pair<std::string, std::unique_ptr<Sound>>(sound->name(),std::move(sound)));

	}

	void ResourceManager::addMusic(std::unique_ptr<Music> music) {
		_loadedMusic.insert(std::pair<std::string, std::unique_ptr<Music>>(music->name(), std::move(music)));
	}


	Sound* ResourceManager::getSound(std::string name) {
		auto& it = _loadedSounds.find(name);

		if (it == std::end(_loadedSounds)) {
			warn("Sound ", name, " not found!");
			return nullptr;
		}
		else {
			return it->second.get();
		}
	}

	Music* ResourceManager::getMusic(std::string name) {

		auto& it = _loadedMusic.find(name);

		if (it == std::end(_loadedMusic)) {
			warn("Music '", name, "' not found!");
			return nullptr;
		}
		else {
			return it->second.get();
		}

	}

	void ResourceManager::addMap(Map map) {
		map.create();
		map.initialize();
		_loadedMaps.insert(std::pair<std::string, Map>(map.name(), map));
	}

	Map ResourceManager::getMap(std::string mapName) {
		auto iter = _loadedMaps.find(mapName);

		if (iter == _loadedMaps.end()) {
			warn("Map '", mapName, "' not found!");

			return Map{};
		}
		else {
			return iter->second;
		}
	}

	void ResourceManager::addParticleEffect(LinearParticleDef& def) {
		_loadedParticleEffects[def.name] = def;
	}

	LinearParticleDef ResourceManager::getParticleEffect(std::string name) {
		return _loadedParticleEffects[name];
	}

	int ResourceManager::loadFont_bind(LuaState& lua) {

		auto f = new Font{};

		f->setName(lua.pullStack<std::string>(1));
		f->setFileSource(lua.pullStack<std::string>(2));
		f->setFontSize(lua.pullStack<int>(3));
		if (lua.pullStack<bool>(5)) {
			f->create();
			f->initialize();
		}

		single<ResourceManager>().addFont(std::unique_ptr<Font>(f));

		auto b = lua.pullStack<bool>(4);

		if (b) single<ResourceManager>().setDefaultFont(f->name());

		return 0;
	}

	int ResourceManager::loadSound_bind(LuaState& lua) {

		auto s = new Sound{};

		s->setName(lua.pullStack<std::string>(1));
		s->setSoundFilePath(lua.pullStack<std::string>(2));
		if (lua.pullStack<bool>(3)) {
			s->create();
			s->initialize();
		}

		single<ResourceManager>().addSound(std::unique_ptr<Sound>(s));

		return 0;
	}

	int ResourceManager::playSound_bind(LuaState& lua) {

		auto soundName = lua.pullStack<std::string>(1);
		
		auto sound = single<ResourceManager>().getSound(soundName);
		if (sound != nullptr) {
			sound->play();		
		}
		return 0;
	}

	int ResourceManager::loadMusic_bind(LuaState& lua) {
		return 0;
	}


	int ResourceManager::loadTexture_bind(LuaState& lua) {

		auto t = new Texture{};

		t->setFileSource(lua["path"]);

		t->setName(lua["name"]);

		if (t->create() != InitStatus::CREATE_TRUE) {
			error("Could not load texture ", t->name());
			return 0;
		}
		if (t->initialize() != InitStatus::INIT_TRUE) {
			error("Could not bind GL texture ", t->name());
			return 0;
		}

		else {
			single<ResourceManager>().addTexture(std::unique_ptr<Texture>(t));
		}
		return 0;
	}

	int ResourceManager::loadAnimationSet_bind(LuaState& lua) {
		auto a = AnimationSet{};

		a.setName(lua["name"]);
		
		auto texture = new Texture{};
		auto textureName = a.name() + "_texture";

		texture->setFileSource(lua["path"]);
		texture->setName(textureName);
		texture->initialize();
		
		single<ResourceManager>().addTexture(std::unique_ptr<Texture>(texture));


		lua.getTable("animations");
		auto animationCount = 1;
		auto animations = std::vector<Animation>{};

		lua.foreachIndex([&](int key, LuaState& lua)->void {
			auto animation = Animation{};
			animation.texture = texture;
			animation.setName(lua["name"]);
			animation.firstLoopIndex = lua["firstLoopFrame"];

			LuaVec2 orientation = lua["orientation"];
			animation.orientation = orientation.getVec2();			
			lua.getTable("frames");
			lua.foreachIndex([&](int key, LuaState& lua)->void {
				

				auto frame = AnimationFrame{};


				LuaVector<int, 5> luaFrame = lua;

				frame.delay = luaFrame[4];


				frame.frame = SDL_Rect{ luaFrame[0] , luaFrame[1], luaFrame[2], luaFrame[3] };

				animation.frames.push_back(frame);
			
			});



			a.addAnimation(animation.name(), animation);

			lua.popStack(1); //pop "frames"

		});
			
		lua.popStack(1); //pop "animations"

		single<ResourceManager>().addAnimationSet(a);	
		return 0;
	}

	int ResourceManager::loadMap_bind(LuaState& lua) {

		auto map = Map{};

		map.setName(lua["name"]);

		map.setTmxFilePath(lua["path"]);

		single<ResourceManager>().addMap(map);


		return 0;
	}

	int ResourceManager::loadParticleEffect_bind(LuaState& lua) {


		std::string name = lua["name"];
		auto effect = LinearParticleDef{};
		effect.fromLua(lua);
		single<ResourceManager>().addParticleEffect(effect);
		
		return 0;
	}


} //end namespace core