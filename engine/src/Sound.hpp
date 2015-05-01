#ifndef CORE_SOUND_H
#define CORE_SOUND_H

#include <string>
#include "Resource.hpp"
#include "Templates.hpp"
#include "Console.hpp"


namespace core {



	class Sound : public Resource, public pausable<Sound> {

	public:

		Sound() : _duration{ 0 }, _soundFilePath{ "" } {

			_sdlSound = nullptr;

		}

		std::string soundFilePath() const {
			return _soundFilePath;
		}

		void setSoundFilePath(std::string soundFile) {
			_soundFilePath = soundFile;
		}

		int duration() const {
			return _duration;
		}

		void resumeImpl() {
			play();
		}

		void PauseImpl() {

		}

		void play() {
			Mix_PlayChannel(-1, _sdlSound, 0);
		}

		void setVolumeLevel(int volume) {
			Mix_VolumeChunk(_sdlSound, volume);

		}

		bool createImpl() override {
			_sdlSound = Mix_LoadWAV(_soundFilePath.c_str());
			if (_sdlSound == nullptr) {
				warn("Could not load sound '", _name, "' at ", _soundFilePath, ": ", SDL_GetError());
				return false;
			}

			return true;

		}

		bool initializeImpl() override {
			setVolumeLevel(10);
			return true;
		}

		bool resetImpl() override {
			return true;
		}

		bool destroyImpl() override {
			Mix_FreeChunk(_sdlSound);
			_sdlSound = nullptr;
			return false;
		}

		virtual ~Sound() {
			reset();
			destroy();
		}
		

	private:
		int _duration;

		int _volumeLevel;

		std::string _soundFilePath;

		Mix_Chunk* _sdlSound;


	};

}


#endif