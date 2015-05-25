#include "AudioManager.hpp"
#include "Console.hpp"
#include "Core.hpp"
#include "Math.hpp"

namespace core {


	bool AudioManager::createImpl() {
		info("Creating AudioManager:");

		info("Initializing SDL_audio...");

		if (SDL_Init(SDL_INIT_AUDIO) < 0)  {
			error("Error initializing SDL audio: ", SDL_GetError());
			return false;
		}


		auto& config = single<Config>();
		int samplingFrequency = config.audio.samplingFrequency;
		bool stereo = config.audio.stereo;
		int chunkSize = config.audio.chunkSize;
		auto outputDimension = (stereo) ? 2 : 1;
		info("Initializing SDL_mixer...");

		if (Mix_OpenAudio(samplingFrequency, MIX_DEFAULT_FORMAT, outputDimension, chunkSize) < 0) {
			error("Error initializing SDL_mixer: ", Mix_GetError());
			return false;
		}
		info("Registering lua functions...");

		//register lua functions
		auto& lua = single<Core>().lua();
		lua.bindFunction("playSound_bind", playSound_bind);
		lua.bindFunction("loadSound_bind", loadSound_bind);
		lua.bindFunction("setGlobalVolumeLevel_bind", setGlobalVolumeLevel_bind);
		lua.bindFunction("getGlobalVolumeLevel_bind", getGlobalVolumeLevel_bind);

		_globalVolumeLevel = 128;

		return true;
	}

	bool AudioManager::initializeImpl() {

		for (auto& sound : _loadedSounds) {
			sound.second.get()->initialize();
		}

		for (auto& music : _loadedMusic) {
			music.second.get()->initialize();
		}
		return true;
	}

	bool AudioManager::resetImpl() {
		for (auto& sound : _loadedSounds) {
			sound.second.get()->reset();
		}

		for (auto& music : _loadedMusic) {
			music.second.get()->reset();
		}
		return true;
	}

	bool AudioManager::destroyImpl() {

		for (auto& sound : _loadedSounds) {
			sound.second.get()->destroy();
		}

		_loadedSounds.clear();

		for (auto& music : _loadedMusic) {
			music.second.get()->destroy();
		}
		_loadedMusic.clear();

		return true;
	}

	void AudioManager::playSound(Sound* sound, float level) {

		level = bounded(level, 0.0f, 2.0f);

		float finalLevel = (_globalVolumeLevel * level) * 128;			
		sound->setVolumeLevel(roundFloat(finalLevel));
		sound->play();

	}


	void AudioManager::addSound(std::unique_ptr<Sound> sound) {
		_loadedSounds.insert(std::pair<std::string, std::unique_ptr<Sound>>(sound->name(), std::move(sound)));

	}

	void AudioManager::addMusic(std::unique_ptr<Music> music) {
		_loadedMusic.insert(std::pair<std::string, std::unique_ptr<Music>>(music->name(), std::move(music)));
	}


	Sound* AudioManager::getSound(std::string name) {
		auto& it = _loadedSounds.find(name);

		if (it == std::end(_loadedSounds)) {
			warn("Sound ", name, " not found!");
			return nullptr;
		}
		else {
			return it->second.get();
		}
	}

	Music* AudioManager::getMusic(std::string name) {

		auto& it = _loadedMusic.find(name);

		if (it == std::end(_loadedMusic)) {
			warn("Music '", name, "' not found!");
			return nullptr;
		}
		else {
			return it->second.get();
		}

	}

	int AudioManager::loadSound_bind(LuaState& lua) {

		auto s = new Sound{};

		s->setName(lua.pullStack<std::string>(1));
		s->setSoundFilePath(lua.pullStack<std::string>(2));
		if (lua.pullStack<bool>(3)) {
			s->create();
			s->initialize();
		}

		single<AudioManager>().addSound(std::unique_ptr<Sound>(s));

		return 0;
	}

	int AudioManager::playSound_bind(LuaState& lua) {

		auto soundName = lua.pullStack<std::string>(1);

		auto sound = single<AudioManager>().getSound(soundName);
		if (sound != nullptr) {
			sound->play();
		}
		return 0;
	}

	int AudioManager::loadMusic_bind(LuaState& lua) {
		return 0;
	}

	void AudioManager::setGlobalVolumeLevel(float level) {
		_globalVolumeLevel = bounded(level,0.0f, 2.0f);
	}

	float AudioManager::getGlobalVolumeLevel() {
		return _globalVolumeLevel;
	}

	int AudioManager::setGlobalVolumeLevel_bind(LuaState& lua) {
		float level = lua.pullStack<float>();
		single<AudioManager>().setGlobalVolumeLevel(level);
		return 0;
	}

	int AudioManager::getGlobalVolumeLevel_bind(LuaState& lua) {
		auto level = single<AudioManager>().getGlobalVolumeLevel();
		lua.pushStack(level);
		return 1;
	}


} //end namespace core