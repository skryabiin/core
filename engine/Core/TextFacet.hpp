#ifndef CORE_TEXT_FACET_HPP
#define CORE_TEXT_FACET_HPP

#include "Facet.hpp"
#include "Geometry.hpp"
#include "RenderedTextToken.hpp"
#include "Font.hpp"

namespace core {



	struct TextFacet : public Facet {

		TextFacet() : offset{ 0, 0, 0 }, position{ 0, 0, 0 }, textContent{ "" }, scale{ 1.0f, 1.0f }, color{ 255, 0, 0, 255 }, drawableId{ -1 } {
			renderedTextToken = nullptr;
			font = nullptr;
		};

		Pixel offset;

		Pixel position;

		std::string textContent;

		RenderedTextToken* renderedTextToken;

		Font* font;

		Vec2 scale;

		Color color;

		int drawableId;

	};



} // end namespace core

#endif