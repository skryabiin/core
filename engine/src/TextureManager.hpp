#ifndef CORE_RESOURCE_MANAGER_HPP
#define CORE_RESOURCE_MANAGER_HPP


#include <vector>
#include <map>
#include <memory>
#include <iterator>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

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









	class TextureManager : public initializable<TextureManager, void, void, void, void>, public singleton<TextureManager> {

	public:

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		Texture* addTexture(Texture* texture);

		Texture* getTexture(std::string name);

		void removeTexture(std::string name);

		AnimationSet getAnimationSet(std::string name) const;

		void addAnimationSet(AnimationSet animationSet);
		
		void replaceAnimationSet(AnimationSet animationSet);

		Font* addFont(Font* font);

		Font* getFont(std::string name);

		Font* getDefaultFont();

		void setDefaultFont(std::string fontName);

		void addMap(Map map);

		Map getMap(std::string mapName);



		void addParticleEffect(LinearParticleDef& def);

		LinearParticleDef getParticleEffect(std::string name);



		static int loadFont_bind(LuaState& lua);


		static int loadAnimationSet_bind(LuaState& lua);

		static int loadTexture_bind(LuaState& lua);

		static int loadMap_bind(LuaState& lua);



		static int loadParticleEffect_bind(LuaState& lua);


	private:

		//atlas
		void _initializeTextureAtlas();

		void _resetTextureAtlas();

		Texture* _atlas;

		
		//loaded resources
		std::map<std::string, Texture*> _loadedTextures;

		std::map<std::string, AnimationSet> _loadedAnimationSets;

		std::map<std::string, Font*> _loadedFonts;

		std::map<std::string, LinearParticleDef> _loadedParticleEffects;

				
		Font* _defaultFont;

		
	};



} //end namespace core


#endif