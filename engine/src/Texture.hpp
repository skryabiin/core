#ifndef CORE_TEXTURE_HPP
#define CORE_TEXTURE_HPP

#include "Console.hpp"
#include "SDL.h"
#include "Resource.hpp"
#include "GL/glew.h"
#include "Color.hpp"

namespace core {

	class Texture : public Resource {
		friend class Renderer;
		friend class TextureManager;
	public:

		Texture() : _sdlSurface{ nullptr } {
			_defaultTextureColor.r = 255;
			_defaultTextureColor.g = 255;
			_defaultTextureColor.b = 255;
			_defaultTextureColor.a = 255;
						
			_glTextureId = -1;
			
		}

		void setSdlSurfaceSource(SDL_Surface* sdlSurface);

		void setFileSource(std::string fileName);

		void modulateTextureColor(Color& color);

		void setDefaultTextureColor(Color& color);

		void restoreDefaultTextureColor();

		void setBlendMode(GLenum blendMode);


		GLuint getGlTextureId() const;

		virtual bool createImpl() override;

		virtual bool initializeImpl() override;

		virtual bool resetImpl() override;

		virtual bool destroyImpl() override;

		virtual ~Texture();

		const SDL_Rect& dimensions() const;

		const SDL_Rect& surfaceTrueDimensions() const;

		bool createFromSurface();

		bool isTextureAtlasManaged();

		const Pixel& getTextureAtlasOrigin();

	private:
		GLenum _blendMode;

		SDL_Surface* _sdlSurface;

		SDL_Rect _surfaceTrueDimensions;
		SDL_Rect _dimensions;

		std::string _fileSource;

		Color _defaultTextureColor;

		bool _isColorModulated;

		GLuint _glTextureId;

		bool _isTextureAtlasManaged;

		Pixel _textureAtlasOrigin;

	};


} //end namespace core

#endif