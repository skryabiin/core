#include <SDL_image.h>

#include "Texture.hpp"





namespace core {



	void Texture::setFileSource(std::string fileName) {
		_fileSource = fileName;
	}

	InitStatus Texture::initializeImpl() {

		if (_fileSource.compare("")) {
			auto sdlSurface = IMG_Load(_fileSource.c_str());
			sdlSurface = (sdlSurface != NULL) ? sdlSurface : nullptr;

			if (sdlSurface == nullptr) return InitStatus::INIT_FAILED;
			_sdlSurface = sdlSurface;
			return (createFromSurfaceNoBlend()) ? InitStatus::INIT_TRUE : InitStatus::INIT_FAILED;			
		}
		else if (_sdlSurface != nullptr) {
			return (createFromSurfaceNoBlend()) ? InitStatus::INIT_TRUE : InitStatus::INIT_FAILED;
		}
		
		return InitStatus::INIT_FALSE;

	}

	void Texture::setSdlSurfaceSource(SDL_Surface* sdlSurface) {
		_sdlSurface = sdlSurface;
	}

	bool Texture::createFromSurfaceLinearBlend() {
		return createFromSurface(GL_LINEAR);
	}

	bool Texture::createFromSurfaceNoBlend() {
		return createFromSurface(GL_NEAREST);
	}

	bool Texture::createFromSurface(GLenum blendMode) {

		_dimensions = SDL_Rect{ 0, 0, _sdlSurface->w, _sdlSurface->h };
		if (!(isPowerOfTwo(_sdlSurface->w)) || !(isPowerOfTwo(_sdlSurface->h))) {

			//convert to gl dimension texture

			int newW = nextPowerOf2(_sdlSurface->w);
			int newH = nextPowerOf2(_sdlSurface->h);

			SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, newW, newH, _sdlSurface->format->BitsPerPixel, _sdlSurface->format->Rmask, _sdlSurface->format->Gmask, _sdlSurface->format->Bmask, _sdlSurface->format->Amask);

			if (tempSurface == NULL) {
				error("Creating surface with new dimensions failed: ", SDL_GetError());
				return false;
			}

			SDL_FillRect(tempSurface, NULL, 0);

			SDL_BlitSurface(_sdlSurface, NULL, tempSurface, &_dimensions);

			SDL_FreeSurface(_sdlSurface);

			_sdlSurface = tempSurface;

		}

		glGenTextures(1, &_glTextureId);

		int internalFormat = (_sdlSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;

		glBindTexture(GL_TEXTURE_2D, _glTextureId);
		//GL_TEXTURE_2D, mipmap index, internal format, w, h, border = 0, format = internal format, data type, data*
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _sdlSurface->w, _sdlSurface->h, 0, internalFormat, GL_UNSIGNED_BYTE, _sdlSurface->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, blendMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, blendMode);

		glBindTexture(GL_TEXTURE_2D, NULL);

		return true;
	}


	InitStatus Texture::resetImpl() {

		glDeleteTextures(1, &_glTextureId);


		SDL_FreeSurface(_sdlSurface);

		_sdlSurface = nullptr;

		return InitStatus::INIT_FALSE;
	}


	SDL_Rect Texture::dimensions() const {

		return _dimensions;
	}


	void Texture::setBlendMode(int mode) {
		switch (mode) {
		case 0:
			setBlendModeNone();
			break;
		case 1:
			setBlendModeBlend();
			break;
		case 2:
			setBlendModeAdd();
			break;
		case 4: 
			setBlendModeMod();
			break;
		}
	}

	void Texture::setBlendModeMod() {
		//SDL_SetTextureBlendMode(_sdlTexture, SDL_BLENDMODE_MOD);
	}

	void Texture::setBlendModeBlend() {
		//SDL_SetTextureBlendMode(_sdlTexture, SDL_BLENDMODE_BLEND);
	}

	void Texture::setBlendModeAdd() {
		//SDL_SetTextureBlendMode(_sdlTexture, SDL_BLENDMODE_ADD);
	}

	void Texture::setBlendModeNone() {
		//SDL_SetTextureBlendMode(_sdlTexture, SDL_BLENDMODE_NONE);
	}

	void Texture::modulateTextureColor(Color& color) {
		//SDL_SetTextureColorMod(_sdlTexture, color.r, color.g, color.b);
		//SDL_SetTextureAlphaMod(_sdlTexture, color.a);
		_isColorModulated = true;
	}

	void Texture::setDefaultTextureColor(Color& color) {
		_defaultTextureColor = color;
		_isColorModulated = true;
	}

	void Texture::restoreDefaultTextureColor() {
		if (!_isColorModulated) return;
		modulateTextureColor(_defaultTextureColor);
		_isColorModulated = false;
	}

	GLuint Texture::getGlTextureId() const {
		return _glTextureId;
	}

	Texture::~Texture() {

		if (reset() == InitStatus::INIT_FALSE) {
			info("Unloaded texture '", _name, "' with source path '", _fileSource, "\n");
			
		}
		else {
			error("Problem unloading texture '", _name, "' with source path '", _fileSource, "\n");
		}

	}





} //end namespace core