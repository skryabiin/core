#include <SDL_image.h>

#include "Texture.hpp"





namespace core {



	void Texture::setFileSource(std::string fileName) {
		_fileSource = fileName;
	}

	bool Texture::createImpl() {

		if (_fileSource.compare("")) {
			auto sdlSurface = IMG_Load(_fileSource.c_str());
			sdlSurface = (sdlSurface != NULL) ? sdlSurface : nullptr;

			if (sdlSurface == nullptr) return false;
			_sdlSurface = sdlSurface;

				
		}	
		return createFromSurface();

	}

	bool Texture::initializeImpl() {
		if (_sdlSurface == nullptr) return false;

		glGenTextures(1, &_glTextureId);

		int internalFormat = (_sdlSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;

		glBindTexture(GL_TEXTURE_2D, _glTextureId);
		//GL_TEXTURE_2D, mipmap index, internal format, w, h, border = 0, format = internal format, data type, data*
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _sdlSurface->w, _sdlSurface->h, 0, internalFormat, GL_UNSIGNED_BYTE, _sdlSurface->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, NULL);
		return true;
	}

	bool Texture::resetImpl() {

		glDeleteTextures(1, &_glTextureId);

		return true;
	}



	bool Texture::destroyImpl() {
		SDL_FreeSurface(_sdlSurface);

		_sdlSurface = nullptr;		
		info("Unloaded texture '", _name, "' with source path '", _fileSource);	
		return true;
	}


	void Texture::setSdlSurfaceSource(SDL_Surface* sdlSurface) {
		_sdlSurface = sdlSurface;
	}

	bool Texture::createFromSurface() {

		_dimensions = SDL_Rect{ 0, 0, _sdlSurface->w, _sdlSurface->h };
		if (!(isPowerOfTwo(_sdlSurface->w)) || !(isPowerOfTwo(_sdlSurface->h))) {

			//convert to gl dimension texture

			int newW = nextPowerOf2(_sdlSurface->w);
			int newH = nextPowerOf2(_sdlSurface->h);

			_surfaceTrueDimensions.x = 0;
			_surfaceTrueDimensions.y = 0;
			_surfaceTrueDimensions.w = newW;
			_surfaceTrueDimensions.h = newH;

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
		else {
			_surfaceTrueDimensions = _dimensions;
		}



		return true;
	}


	SDL_Rect Texture::surfaceTrueDimensions() const {
		return _surfaceTrueDimensions;
	}

	SDL_Rect Texture::dimensions() const {

		return _dimensions;
	}


	void Texture::setBlendMode(GLenum blendMode) {
		_blendMode = blendMode;
	}


	GLuint Texture::getGlTextureId() const {
		return _glTextureId;
	}

	Texture::~Texture() {

		reset();
		destroy();

	}





} //end namespace core