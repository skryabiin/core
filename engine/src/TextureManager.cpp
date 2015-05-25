#include <SDL.h>
#include "Tmx.h"
#include "TextureManager.hpp"
#include "Renderer.hpp"
#include "Util.hpp"
#include "SDLUtil.hpp"
#include "Console.hpp"
#include "Core.hpp"
#include "Font.hpp"
#include "RectSorter.hpp"

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
			texture.second->initialize();
		}

		for (auto& font : _loadedFonts) {
			font.second->initialize();
		}

		_initializeTextureAtlas();
		single<Renderer>().setTextureAtlas(_atlas);
		
		return true;
	}

	bool TextureManager::resetImpl() {
		
		for (auto& texture : _loadedTextures) {
			texture.second->reset();
		}

		for (auto& font : _loadedFonts) {
			font.second->reset();
		}

		_resetTextureAtlas();
		
		//TODO reset everything
		return true;
	}

	bool TextureManager::destroyImpl() {

		for (auto& texture : _loadedTextures) {
			texture.second->destroy();
			delete texture.second;
		}
		_loadedTextures.clear();

		_loadedAnimationSets.clear();

		for (auto& font : _loadedFonts) {
			font.second->destroy();
			delete font.second;
		}
		_loadedFonts.clear();




		//TODO why aren't these resources
		_loadedParticleEffects.clear();
		return true;
	}

	Font* TextureManager::addFont(Font* font) {
		std::string name = font->name();
		_loadedFonts.insert(std::pair<std::string, Font*>(font->name(), font));
		return _loadedFonts.find(name)->second;
	}

	Font* TextureManager::getFont(std::string name) {
		
		auto iter = _loadedFonts.find(name);

		if (iter == _loadedFonts.end()) {
			warn("Font '", name,"' not found!");

			return nullptr;
		}
		else {
			return iter->second;
		}
	}


	Font* TextureManager::getDefaultFont() {
		return _defaultFont;
	}

	void TextureManager::setDefaultFont(std::string fontName) {

		for (auto& font : _loadedFonts) {

			if (!font.second->name().compare(fontName)) {
				_defaultFont = font.second;
				break;
			}
		}
	}


	Texture* TextureManager::addTexture(Texture* texture) {
		std::string name = texture->name();
		_loadedTextures.insert(std::pair<std::string,Texture*>(texture->name(), texture));
		return getTexture(name);
	}

	void TextureManager::removeTexture(std::string name) {
		_loadedTextures.erase(name);
	}

	Texture* TextureManager::getTexture(std::string name) {

		std::map<std::string, Texture*>::iterator iter = _loadedTextures.find(name);

		if (iter == _loadedTextures.end()) {
			warn("Texture '", name, "' not found!");

			return nullptr;
		} else {
			return iter->second;
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

		single<TextureManager>().addFont(f);

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

		else {
			single<TextureManager>().addTexture(t);
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
		texture->create();
		texture->initialize();
		
		single<TextureManager>().addTexture(texture);


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


	void TextureManager::_initializeTextureAtlas() {

		auto rs = RectSorter{};		
		auto rect = SDL_Rect();
		int depth = 0;
		for (auto& texture : _loadedTextures) {
			rect = texture.second->dimensions();
			rs.addRect(texture.second->id(), rect);
			
		}

		Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
#endif

		auto sr = rs.sort();
		auto atlasSurface = SDL_CreateRGBSurface(0, sr.dimension, sr.dimension, 32, rmask, gmask, bmask, amask);

		SDL_FillRect(atlasSurface, NULL, 0);

		auto srcRect = SDL_Rect();
		for (auto& texture : _loadedTextures) {
			rect = rs.getRect(texture.second->id());
			srcRect = texture.second->dimensions();
			texture.second->_textureAtlasOrigin = Pixel{ rect.x, rect.y, 0 };
			texture.second->_isTextureAtlasManaged = true;
			SDL_BlitSurface(texture.second->_sdlSurface, &srcRect, atlasSurface, &rect);
			texture.second->destroy();
		}

		_atlas = new Texture{};
		_atlas->setSdlSurfaceSource(atlasSurface);
		_atlas->create();
		_atlas->initialize();


	}

	void TextureManager::_resetTextureAtlas() {
		_atlas->reset();
		_atlas->destroy();
		delete _atlas;
	}


} //end namespace core