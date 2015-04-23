#ifndef CORE_MUSIC_HPP
#define CORE_MUSIC_HPP

#include <SDL_mixer.h>
#include "Resource.hpp"

namespace core {


	class Music : public Resource, public pausable<Music> {

	public:

		Music() : _duration{ 0 }, _musicFilePath{ "" } {
			_sdlMusic = nullptr;
		}

		std::string musicFilePath() const {
			return _musicFilePath;
		}

		void setMusicFilePath(std::string musicFile) {
			_musicFilePath = musicFile;
		}


		void resumeImpl() {
			if (Mix_PlayingMusic() == 0) {
				Mix_PlayMusic(_sdlMusic, -1);
			}
		}

		void pauseImpl() {

			if (Mix_PlayingMusic() != 0) {
				Mix_PauseMusic();
			}
		}

		void stop() {
			Mix_HaltMusic();
		}

		int duration() const {
			return _duration;
		}

		virtual bool load() {
			_sdlMusic = Mix_LoadMUS(_musicFilePath.c_str());
			return (_sdlMusic != nullptr);
		}

		virtual bool unload() {
			Mix_FreeMusic(_sdlMusic);
			return true;
		}

		virtual ~Music() {
			unload();
		}

	private:

		int _duration;

		std::string _musicFilePath;

		Mix_Music* _sdlMusic;

	};



} //end namespace core

#endif