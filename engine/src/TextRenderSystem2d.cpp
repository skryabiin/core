#include <functional>
#include "Renderer.hpp"
#include "RenderedTextToken.hpp"
#include "TextRenderSystem2d.hpp"
#include "Drawable.hpp"
#include "TextContentChangeEvent.hpp"
#include "Core.hpp"
#include "OffsetChangeEvent.hpp"

namespace core {




	TextRenderSystem2d::TextRenderSystem2d() {


	}

	bool TextRenderSystem2d::createImpl() {
		return RenderableSystem2d::createImpl();
	}

	bool TextRenderSystem2d::initializeImpl() {
	

		return RenderableSystem2d::initializeImpl();
	}

	bool TextRenderSystem2d::resetImpl() {

		for (auto& facet : _textFacets) {
			single<Renderer>().destroyDrawable(facet.drawable);
		}
		_textFacets.clear();
		_renderedTextTokens.clear();
		
		return RenderableSystem2d::resetImpl();
	}

	bool TextRenderSystem2d::destroyImpl() {
		return RenderableSystem2d::destroyImpl();
	}
	


	bool TextRenderSystem2d::handleEvent(ColorChangeEvent& colorChangeEvent) {
		return true;
	}



	std::vector<Facet*> TextRenderSystem2d::getFacets(Entity& e) {

		auto out = std::vector<Facet*>{};

		for (auto& facet : _textFacets) {
			if (facet.of() == e) {
				out.push_back(&facet);
			}
		}
		return out;
	}

	
	TextFacet& TextRenderSystem2d::createTextFacet(Entity& e, std::string fontName, Pixel position, Pixel offset, Vec2 scale, Color color, std::string text) {


		auto facet = TextFacet{};
		facet.setOf(e);
		facet.offset = offset;
		facet.color = color;
		facet.textContent = text;		
		if (!fontName.compare("default")) {
			single<ResourceManager>().getDefaultFont();
		} else {
			facet.font = single<ResourceManager>().getFont(fontName);
		}
		
		auto rtt = facet.font->getRenderedTextToken();
		rtt->setText(text);
		facet.renderedTextToken = rtt.get();
		_renderedTextTokens.push_back(std::move(rtt));
	
	
		facet.drawable.sourceRect = facet.renderedTextToken->texture()->dimensions();
		facet.drawable.targetRect.x = position.x + offset.x;
		facet.drawable.targetRect.y = position.y + offset.y;
		facet.drawable.targetRect.w = facet.drawable.sourceRect.w * scale.x;
		facet.drawable.targetRect.h = facet.drawable.sourceRect.h * scale.y;
		facet.drawable.zIndex = position.z + offset.z;
		facet.drawable.layerId = _drawableLayerId;
		facet.drawable.drawableType = Drawable::DrawableType::TEXTURE;
		facet.drawable.camera = getCamera();
		facet.drawable.texture = facet.renderedTextToken->texture();
		facet.drawable.id = single<Renderer>().createDrawable(facet.drawable);


		_textFacets.push_back(std::move(facet));
		return _textFacets.back();
	}


	bool TextRenderSystem2d::handleEvent(FacetDimensionQuery& facetDimensionQuery) {

		for (auto& facet : _textFacets) {
			if (facet.of() == facetDimensionQuery.entity && facet.id() == facetDimensionQuery.facetId) {

				SDL_Rect dim = facet.drawable.targetRect;				
				facetDimensionQuery.dimensions.setDimension(Dimension{ dim.w, dim.h });
				facetDimensionQuery.found = true;
				return false;
			}
		}
		return true;
	}


	bool TextRenderSystem2d::handleEvent(TextContentChangeEvent& textContentChange) {

		for (auto& facet : _textFacets) {
			if (facet.of() == textContentChange.entity && facet.id() == textContentChange.facetId) {
				setText(facet, textContentChange.font, textContentChange.textContent);
				return false;
			}
		}
		return true;
	}

	void TextRenderSystem2d::destroyFacets(Entity& entity) {

		RenderedTextToken* renderedToken = nullptr;

		for (auto it = std::begin(_textFacets); it != std::end(_textFacets); ++it) {
			if (it->of() == entity) {
				renderedToken = it->renderedTextToken;				
				single<Renderer>().destroyDrawable(it->drawable);
				_textFacets.erase(it);
				break;
			}
		}

		if (renderedToken != nullptr) {

			for (auto it = std::begin(_renderedTextTokens); it != std::end(_renderedTextTokens); ++it) {

				if (it->get() == renderedToken) {
					_renderedTextTokens.erase(it);
					return;
				}

			}
		}
	}


	void TextRenderSystem2d::setText(TextFacet& facet, std::string font ,std::string text) {
		facet.textContent = text;

		if (facet.renderedTextToken == nullptr) {
			Font* font = nullptr;
			if (facet.font != nullptr) {
				font = facet.font;
			}
			else if (_defaultFont != nullptr) {
				font = _defaultFont;
			}
			if (font != nullptr) {
				auto rtt = font->getRenderedTextToken();
				rtt->setText(text);
				facet.renderedTextToken = rtt.get();
				_renderedTextTokens.push_back(std::move(rtt));
			}
		}
		else {
			facet.renderedTextToken->setText(text);
		}
				
		facet.drawable.sourceRect = facet.renderedTextToken->texture()->dimensions();
		facet.drawable.targetRect.w = facet.drawable.sourceRect.w;
		facet.drawable.targetRect.h = facet.drawable.sourceRect.h;
		facet.drawable.texture = facet.renderedTextToken->texture();
		single<Renderer>().updateDrawable(facet.drawable);
	}


	void TextRenderSystem2d::setDefaultFont(Font* defaultFont) {
		_defaultFont = defaultFont;
	}


	int TextRenderSystem2d::createFacet_bind(LuaState& lua) {
		
		auto systemName = lua.pullStack<std::string>(1);
		auto entity = lua.pullStack<int>(2);

		auto system = single<Core>().getSystemByName<TextRenderSystem2d>(systemName);

		if (system != nullptr) {


			info("Creating text facet for entity ", entity);

			std::string fontName = lua["font"];

			LuaPixel offset = lua["offset"];

			LuaPixel position = lua["position"];

			LuaColor color = lua["color"];

			std::string text = lua["text"];	

			LuaVec2 scale = lua["scale"];

			auto& newFacet = system->createTextFacet(entity, fontName, position.getPixel(), offset.getPixel(), scale.getVec2(), color.getColor(), text);
			lua.pushStack(newFacet.id());
		}
		else {
			lua.pushStack(-1L);
		}


		return 1;
	}

}