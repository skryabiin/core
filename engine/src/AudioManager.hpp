#ifndef CORE_AUDIO_MANAGER_HPP
#define CORE_AUDIO_MANAGER_HPP

#include <SDL.h>
#include <SDL_mixer.h>
#include "Templates.hpp"
#include "Sound.hpp"
#include "Music.hpp"
namespace core {

	struct PlayingSound {
		int soundId;
		int startTick;
		int channelId;

	};


	class AudioManager : public singleton<AudioManager>, public initializable<AudioManager, void, void, void, void> {

	public:

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		void addSound(std::unique_ptr<Sound> sound);

		void addMusic(std::unique_ptr<Music> music);

		Sound* getSound(std::string name);

		Music* getMusic(std::string name);

		void playSound(Sound* sound, float level);

		static int playSound_bind(LuaState& lua);

		static int loadSound_bind(LuaState& lua);

		static int loadMusic_bind(LuaState& lua);

		void setGlobalVolumeLevel(float level);

		float getGlobalVolumeLevel();

		static int setGlobalVolumeLevel_bind(LuaState& lua);

		static int getGlobalVolumeLevel_bind(LuaState& lua);

	private:

		float _globalVolumeLevel;

		std::map<std::string, std::unique_ptr<Sound>> _loadedSounds;

		std::map<std::string, std::unique_ptr<Music>> _loadedMusic;

	};

} //end namespace core

#endif