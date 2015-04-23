#ifndef CORE_TEXTURE_HPP
#define CORE_TEXTURE_HPP

#include "Console.hpp"
#include "SDL.h"
#include "Resource.hpp"
#include "GL/glew.h"


namespace core {

	class Texture : public Resource {
		friend class Renderer;
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

		void setBlendMode(int mode);

		void setBlendModeMod();

		void setBlendModeBlend();

		void setBlendModeAdd();

		void setBlendModeNone();

		GLuint getGlTextureId() const;

		virtual InitStatus initializeImpl() override;

		virtual InitStatus resetImpl() override;

		virtual ~Texture();

		SDL_Rect dimensions() const;

		bool createFromSurfaceLinearBlend();

		bool createFromSurfaceNoBlend();

		bool createFromSurface(GLenum blendMode);

	private:


		SDL_Surface* _sdlSurface;

		SDL_Rect _dimensions;

		std::string _fileSource;

		Color _defaultTextureColor;

		bool _isColorModulated;

		GLuint _glTextureId;

	};


} //end namespace core

#endif