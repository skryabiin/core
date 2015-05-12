#include <SDL.h>
#include "Tmx.h"
#include "TextureManager.hpp"
#include "Renderer.hpp"
#include "Util.hpp"
#include "SDLUtil.hpp"
#include "Console.hpp"
#include "Core.hpp"
#include "Font.hpp"


namespace core {



	bool TextureManager::createImpl() {

		info("Creating TextureManager:");

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

		info("Registering lua functions...");

		//register lua functions
		auto& lua = single<Core>().lua();
		lua.bindFunction("loadFont_bind", loadFont_bind);		
		lua.bindFunction("loadTexture_bind", loadTexture_bind);
		lua.bindFunction("loadAnimationSet_bind", loadAnimationSet_bind);			
		lua.bindFunction("loadParticleEffect_bind", loadParticleEffect_bind);

		return true;
	}

	bool TextureManager::initializeImpl() {
		for (auto& texture : _loadedTextures) {
			texture.second.get()->initialize();
		}

		for (auto& font : _loadedFonts) {
			font.second.get()->initialize();
		}



		
		return true;
	}

	bool TextureManager::resetImpl() {
		
		for (auto& texture : _loadedTextures) {
			texture.second.get()->reset();
		}

		for (auto& font : _loadedFonts) {
			font.second.get()->reset();
		}


		
		//TODO reset everything
		return true;
	}

	bool TextureManager::destroyImpl() {

		for (auto& texture : _loadedTextures) {
			texture.second.get()->destroy();
		}
		_loadedTextures.clear();

		_loadedAnimationSets.clear();

		for (auto& font : _loadedFonts) {
			font.second.get()->destroy();
		}
		_loadedFonts.clear();




		//TODO why aren't these resources
		_loadedParticleEffects.clear();
		return true;
	}

	Font* TextureManager::addFont(std::unique_ptr<Font> font) {
		std::string name = font->name();
		_loadedFonts.insert(std::pair<std::string, std::unique_ptr<Font>>(font->name(), std::move(font)));
		return _loadedFonts.find(name)->second.get();
	}

	Font* TextureManager::getFont(std::string name) {
		
		auto iter = _loadedFonts.find(name);

		if (iter == _loadedFonts.end()) {
			warn("Font '", name,"' not found!");

			return nullptr;
		}
		else {
			return iter->second.get();
		}
	}


	Font* TextureManager::getDefaultFont() {
		return _defaultFont;
	}

	void TextureManager::setDefaultFont(std::string fontName) {

		for (auto& font : _loadedFonts) {

			if (!font.second->name().compare(fontName)) {
				_defaultFont = font.second.get();
				break;
			}
		}
	}


	Texture* TextureManager::addTexture(std::unique_ptr<Texture> texture) {
		std::string name = texture->name();
		_loadedTextures.insert(std::pair<std::string, std::unique_ptr<Texture>>(texture->name(), std::move(texture)));
		return getTexture(name);
	}

	void TextureManager::removeTexture(std::string name) {
		_loadedTextures.erase(name);
	}

	Texture* TextureManager::getTexture(std::string name) {

		std::map<std::string, std::unique_ptr<Texture>>::iterator iter = _loadedTextures.find(name);

		if (iter == _loadedTextures.end()) {
			warn("Texture '", name, "' not found!");

			return nullptr;
		} else {
			return iter->second.get();
		}

	}

	void TextureManager::addAnimationSet(AnimationSet animationSet) {
		_loadedAnimationSets.insert(std::pair<std::string, AnimationSet>(animationSet.name(), animationSet));
	}

	AnimationSet TextureManager::getAnimationSet(std::string name) const {

		auto& it = _loadedAnimationSets.find(name);
		if (it == end(_loadedAnimationSets)) {
			warn("AnimationSet '", name, "' not found!");			
			return AnimationSet{};
		}
		else {
			return it->second;
		}

	}


	

	void TextureManager::addParticleEffect(LinearParticleDef& def) {
		_loadedParticleEffects[def.name] = def;
	}

	LinearParticleDef TextureManager::getParticleEffect(std::string name) {
		return _loadedParticleEffects[name];
	}

	int TextureManager::loadFont_bind(LuaState& lua) {

		auto f = new Font{};

		f->setName(lua.pullStack<std::string>(1));
		f->setFileSource(lua.pullStack<std::string>(2));
		f->setFontSize(lua.pullStack<int>(3));
		if (lua.pullStack<bool>(5)) {
			f->create();
			f->initialize();
		}

		single<TextureManager>().addFont(std::unique_ptr<Font>(f));

		auto b = lua.pullStack<bool>(4);

		if (b) single<TextureManager>().setDefaultFont(f->name());

		return 0;
	}


	int TextureManager::loadTexture_bind(LuaState& lua) {

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
			single<TextureManager>().addTexture(std::unique_ptr<Texture>(t));
		}
		return 0;
	}

	int TextureManager::loadAnimationSet_bind(LuaState& lua) {
		auto a = AnimationSet{};

		a.setName(lua["name"]);
		
		auto texture = new Texture{};
		auto textureName = a.name() + "_texture";

		texture->setFileSource(lua["path"]);
		texture->setName(textureName);
		texture->initialize();
		
		single<TextureManager>().addTexture(std::unique_ptr<Texture>(texture));


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

		single<TextureManager>().addAnimationSet(a);	
		return 0;
	}

	

	int TextureManager::loadParticleEffect_bind(LuaState& lua) {


		std::string name = lua["name"];
		auto effect = LinearParticleDef{};
		effect.fromLua(lua);
		single<TextureManager>().addParticleEffect(effect);
		
		return 0;
	}


} //end namespace core