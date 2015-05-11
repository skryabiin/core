#ifndef CORE_SOUND_H
#define CORE_SOUND_H

#include <string>
#include "Resource.hpp"
#include "Templates.hpp"
#include "Console.hpp"
#include <SDL_mixer.h>

namespace core {



	class Sound : public Resource, public pausable<Sound> {

	public:

		Sound();

		std::string soundFilePath() const;

		void setSoundFilePath(std::string soundFile);

		int duration() const;

		void resumeImpl();
		void pauseImpl();

		int play();

		void setVolumeLevel(int volume);

		bool createImpl();
	
		bool initializeImpl();

		bool resetImpl();

		bool destroyImpl();

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