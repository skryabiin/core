#include "Font.hpp"
#include "RenderedTextToken.hpp"
#include "TextureManager.hpp"
#include "Renderer.hpp"
#include "SDLUtil.hpp"
#include "Util.hpp"

namespace core {



	void Font::setFileSource(std::string fileName) {
		_fileSource = fileName;
	}

	bool Font::createImpl() {

		_ttfFont = TTF_OpenFont(_fileSource.c_str(), _fontSize);
		return (_ttfFont != nullptr) ? true : false;
	}

	bool Font::initializeImpl() {
		return true;
	}

	bool Font::resetImpl() {	
		return true;
	}

	bool Font::destroyImpl() {
		TTF_CloseFont(_ttfFont);
		_ttfFont = nullptr;
		return true;
	}

	void Font::setFontSize(int size) {
		_fontSize = size;
	}

	int Font::fontSize() {
		return _fontSize;
	}

	RenderedTextToken* Font::getRenderedTextToken() {
		auto token = new RenderedTextToken{};
		token->setFont(this);
		return token;
	}

	void Font::renderText(RenderedTextToken* token) {
		if (getInitializedStatus() != InitStatus::INIT_TRUE) return;
		auto texture = token->_texture;
		if (texture == nullptr) {
			texture = new Texture{};
		}
		else {
			texture->reset();
			texture->destroy();
		}		

		auto tempSurf = TTF_RenderText_Blended(_ttfFont, token->_text.c_str(), colorToSdl(token->_textColor));
		texture->setSdlSurfaceSource(tempSurf);
		texture->setName("renderedText-" + std::string{ ltos(token->id()) });
		texture->create();
		texture->initialize();
		if (token->_texture == nullptr) {
			token->_texture = singleton<TextureManager>::instance().addTexture(texture);
		}
	}




} //end namespace core