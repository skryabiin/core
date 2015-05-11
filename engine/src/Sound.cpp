#include "Sound.hpp"
#include "AudioManager.hpp"
#include "Core.hpp"

namespace core {



	Sound::Sound() : _duration{ 0 }, _soundFilePath{ "" } {

		_sdlSound = nullptr;

	}

	std::string Sound::soundFilePath() const {
		return _soundFilePath;
	}

	void Sound::setSoundFilePath(std::string soundFile) {
		_soundFilePath = soundFile;
	}

	int Sound::duration() const {
		return _duration;
	}

	void Sound::resumeImpl() {
		play();
	}

	void Sound::pauseImpl() {

	}

	int Sound::play() {
		return Mix_PlayChannel(-1, _sdlSound, 0);
	}

	void Sound::setVolumeLevel(int volume) {
		Mix_VolumeChunk(_sdlSound, volume);

	}

	bool Sound::createImpl()  {
		_sdlSound = Mix_LoadWAV(_soundFilePath.c_str());
		auto alen = _sdlSound->alen;
		float samples = alen / 2.0f;
		float frequency = single<Core>().lua()("Config")["audio"]["samplingFrequency"];
		_duration = (samples / frequency) * 1000;
		if (_sdlSound == nullptr) {
			warn("Could not load sound '", _name, "' at ", _soundFilePath, ": ", SDL_GetError());
			return false;
		}

		return true;

	}

	bool Sound::initializeImpl()  {
		setVolumeLevel(10);
		return true;
	}

	bool Sound::resetImpl()  {
		return true;
	}

	bool Sound::destroyImpl()  {
		Mix_FreeChunk(_sdlSound);
		_sdlSound = nullptr;
		return false;
	}



} //end namespace core