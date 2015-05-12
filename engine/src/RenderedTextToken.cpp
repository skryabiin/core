#include "RenderedTextToken.hpp"
#include "TextureManager.hpp"
#include "Font.hpp"

namespace core {



	void RenderedTextToken::renderAgain() {
		if (_hasChanged && _font != nullptr) {
			_font->renderText(this);
			_hasChanged = false;
		}
	}
	RenderedTextToken::~RenderedTextToken() {
		if (_texture != nullptr) {
			single<TextureManager>().removeTexture(_texture->name());
		}
	}

	Texture* RenderedTextToken::texture() {
		return _texture;
	}

	void RenderedTextToken::setFont(Font* font) {
		_font = font;
	}


	void RenderedTextToken::setTextColor(Color c) {
		_textColor = c;
		_hasChanged = true;
		renderAgain();
	}

	Color RenderedTextToken::textColor() const {
		return _textColor;
	}

	void RenderedTextToken::setText(std::string text) {
		if (!_text.compare(text)) return;
		_text = text;
		_hasChanged = true;
		renderAgain();
	}

	bool RenderedTextToken::hasChanged() const {
		return _hasChanged;
	}

	std::string RenderedTextToken::text() const {
		return _text;
	}



} //end namespace core