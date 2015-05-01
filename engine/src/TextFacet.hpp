#ifndef CORE_TEXT_FACET_HPP
#define CORE_TEXT_FACET_HPP

#include "VisualFacet.hpp"
#include "Geometry.hpp"
#include "RenderedTextToken.hpp"
#include "Font.hpp"
#include "Drawable.hpp"

namespace core {



	struct TextFacet : public VisualFacet {

		TextFacet() : textContent{ "" }, color{ Color::CommonColor::WHITE } {

			setDeclaredTypeInfo(&typeid(*this));
			renderedTextToken = nullptr;
			font = nullptr;
		};		

		std::string textContent;

		RenderedTextToken* renderedTextToken;

		Font* font;		

		Color color;

	};



} // end namespace core

#endif