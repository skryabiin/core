#ifndef CORE_TEXT_HPP
#define CORE_TEXT_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include "Templates.hpp"
#include "Texture.hpp"


namespace core {




	class Font : public Resource {
		friend class Renderer;
		friend class RenderedTextToken;
	public:

		Font() : Resource{}, _fileSource{ "" }, _fontSize{ 28 } {
			_ttfFont = nullptr;
		};

		void setFileSource(std::string fileName);

		bool createImpl() override;

		bool initializeImpl() override;

		bool resetImpl() override;

		bool destroyImpl() override;

		void setFontSize(int size);

		int fontSize();

		std::unique_ptr<RenderedTextToken> getRenderedTextToken();

		void renderText(RenderedTextToken* token);

	private:		

		std::string _fileSource;

		int _fontSize;

		TTF_Font* _ttfFont;

	};


} //end namespace core


#endif
