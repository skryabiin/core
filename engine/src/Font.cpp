#include "Font.hpp"
#include "RenderedTextToken.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"
#include "SDLUtil.hpp"
#include "Util.hpp"

namespace core {



	void Font::setFileSource(std::string fileName) {
		_fileSource = fileName;
	}

	InitStatus Font::initializeImpl() {
		
		_ttfFont = TTF_OpenFont(_fileSource.c_str(), _fontSize);
		return (_ttfFont != nullptr) ? InitStatus::INIT_TRUE : InitStatus::INIT_FAILED;		
	}

	InitStatus Font::resetImpl() {		

		TTF_CloseFont(_ttfFont);
		_ttfFont = nullptr;

		return InitStatus::INIT_FALSE;
	}



	void Font::setFontSize(int size) {
		_fontSize = size;
	}

	int Font::fontSize() {
		return _fontSize;
	}

	std::unique_ptr<RenderedTextToken> Font::getRenderedTextToken() {
		auto token = new RenderedTextToken{};
		token->setFont(this);
		return std::move(std::unique_ptr<RenderedTextToken>(token));
	}

	void Font::renderText(RenderedTextToken* token) {
		if (getInitializedStatus() != InitStatus::INIT_TRUE) return;

		auto texture = (token->_texture != nullptr) ? token->_texture : new Texture{};

		auto tempSurf = TTF_RenderText_Blended(_ttfFont, token->_text.c_str(), colorToSdl(token->_textColor));
		texture->setSdlSurfaceSource(tempSurf);
		texture->createFromSurfaceLinearBlend();
		texture->setName(std::string{ ltos(token->id()) });
		if (token->_texture == nullptr) {
			token->_texture = singleton<ResourceManager>::instance().addTexture(std::unique_ptr<Texture>(texture));
		}
	}




} //end namespace core