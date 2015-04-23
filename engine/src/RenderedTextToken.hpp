#ifndef CORE_RENDERED_TEXT_TOKEN_HPP
#define CORE_RENDERED_TEXT_TOKEN_HPP

#include "Templates.hpp"
#include "Texture.hpp"

namespace core {


	class RenderedTextToken : public equal_comparable<RenderedTextToken> {
		friend class Font;
	public:
		RenderedTextToken() : _textColor{ Color{ 0xFF, 0xFF, 0xFF, 0xFF } }, _text{ "" } {
			_texture = nullptr;
			_font = nullptr;
		};

		~RenderedTextToken();

		Texture* texture();

		void setFont(Font* font);

		void setTextColor(Color c);

		Color textColor() const;

		void setText(std::string text);

		bool hasChanged() const;

		std::string text() const;

	private:

		void renderAgain();
		bool _hasChanged;
		std::string _text;
		Font* _font;
		Texture* _texture;
		Color _textColor;
	};

} //end namespace core

#endif