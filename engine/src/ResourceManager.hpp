#ifndef CORE_RESOURCE_MANAGER_HPP
#define CORE_RESOURCE_MANAGER_HPP


#include <vector>
#include <map>
#include <memory>
#include <iterator>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "lua.hpp"
#include "Templates.hpp"
#include "Geometry.hpp"
#include "Config.hpp"
#include "AnimationSet.hpp"
#include "Sound.hpp"
#include "Music.hpp"
#include "Map.hpp"
#include "Font.hpp"
#include "LinearParticleField.hpp"


namespace core {









	class ResourceManager : public initializable<ResourceManager, void, void, void, void>, public singleton<ResourceManager> {

	public:

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		Texture* addTexture(std::unique_ptr<Texture> texture);

		Texture* getTexture(std::string name);

		void removeTexture(std::string name);

		AnimationSet getAnimationSet(std::string name) const;

		void addAnimationSet(AnimationSet animationSet);
		
		void replaceAnimationSet(AnimationSet animationSet);

		Font* addFont(std::unique_ptr<Font> font);

		Font* getFont(std::string name);

		Font* getDefaultFont();

		void setDefaultFont(std::string fontName);

		void addMap(Map map);

		Map getMap(std::string mapName);

		void addSound(std::unique_ptr<Sound> sound);

		void addMusic(std::unique_ptr<Music> music);

		Sound* getSound(std::string name);

		Music* getMusic(std::string name);

		void addParticleEffect(LinearParticleDef& def);

		LinearParticleDef getParticleEffect(std::string name);



		static int loadFont_bind(LuaState& lua);

		static int loadSound_bind(LuaState& lua);

		static int loadMusic_bind(LuaState& lua);

		static int loadAnimationSet_bind(LuaState& lua);

		static int loadTexture_bind(LuaState& lua);

		static int loadMap_bind(LuaState& lua);

		static int playSound_bind(LuaState& lua);

		static int loadParticleEffect_bind(LuaState& lua);


	private:

		std::map<std::string, std::unique_ptr<Texture>> _loadedTextures;

		std::map<std::string, AnimationSet> _loadedAnimationSets;

		std::map<std::string, std::unique_ptr<Font>> _loadedFonts;

		std::map<std::string, std::unique_ptr<Sound>> _loadedSounds;

		std::map<std::string, std::unique_ptr<Music>> _loadedMusic;

		std::map<std::string, Map> _loadedMaps;

		std::map<std::string, LinearParticleDef> _loadedParticleEffects;


		Font* _defaultFont;

	};



} //end namespace core


#endif