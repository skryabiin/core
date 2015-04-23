#include <functional>
#include "Renderer.hpp"
#include "RenderedTextToken.hpp"
#include "TextRenderSystem2d.hpp"
#include "Drawable.hpp"
#include "TextContentChange.hpp"
#include "Core.hpp"

namespace core {




	TextRenderSystem2d::TextRenderSystem2d() {


	}

	InitStatus TextRenderSystem2d::initializeImpl() {
		std::function<void(TextRenderSystem2d*, TextContentChange&)> callback = std::mem_fn(&TextRenderSystem2d::handleTextContentChange);
		_textContentChangeFilter.init(this, callback);
		single<EventProcessor>().addFilter(&_textContentChangeFilter);

		std::function<void(TextRenderSystem2d*, FacetDimensionQuery&)> dimensionQueryCallback = std::mem_fn(&TextRenderSystem2d::handleFacetDimensionQuery);
		_facetDimensionQueryFilter.init(this, dimensionQueryCallback);
		single<EventProcessor>().addFilter(&_facetDimensionQueryFilter);

		return RenderableSystem2d::initializeImpl();
	}

	InitStatus TextRenderSystem2d::resetImpl() {
		single<EventProcessor>().removeFilter(&_textContentChangeFilter);
		single<EventProcessor>().removeFilter(&_facetDimensionQueryFilter);
		for (auto& facet : _textFacets) {
			single<Renderer>().destroyDrawable(facet.drawableId);
		}
		_textFacets.clear();
		_renderedTextTokens.clear();
		
		return RenderableSystem2d::resetImpl();
	}
	/*
	void TextRenderSystem2d::renderImpl() {

		auto d = Drawable{};

		int x, y;

		for (auto& text : _textFacets) {

			if (text.isPaused() || (_currentColorId >= 0 && text.colorId >= 0 && text.colorId != _currentColorId)) continue;
			if (text.renderedTextToken == nullptr) continue;

			d.texture = text.renderedTextToken->texture();
			d.sourceRect = d.texture->dimensions();
			x = text.position.x + text.offset.x;
			y = text.position.y + text.offset.y;
			d.targetRect = SimpleRect<int>{ x, y,x + d.sourceRect.width(), y + d.sourceRect.height() };
			d.zIndex = text.position.z;
			d.scale = text.scale;			

			single<Renderer>().render(d, _camera);

		}


	} */


	void TextRenderSystem2d::updateDrawPosition(PositionChangeEvent& positionChange) {
		


		for (auto& facet : _textFacets) {
			if (facet.of() == positionChange.entity) {			

				
				auto drawable = single<Renderer>().getDrawable(facet.drawableId);				
				auto base = (positionChange.relative) ? facet.position : Pixel{};
				auto p = positionChange.position.getPixel() + base;
				facet.position = p;
				if (_cameraFollow.of() == positionChange.entity && !_cameraFollow.isPaused()) {					
					auto x = p.x + drawable.targetRect.w * 0.5f;
					auto y = p.y + drawable.targetRect.h * 0.5f;
					snapCameraToCoordinates(x, y);
				}

				drawable.targetRect.x = p.x + facet.offset.x;
				drawable.targetRect.y = p.y + facet.offset.y;
				drawable.zIndex = p.z + facet.offset.z;

				single<Renderer>().updateDrawable(drawable);
				return;
			}
		}
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

	void TextRenderSystem2d::updateTextFacet(TextFacet& facet, std::string fontName, Pixel position, Pixel offset, Color color, std::string text) {

		if (!fontName.compare("default")) {
			facet.font = single<ResourceManager>().getDefaultFont();
		}
		else {
			facet.font = single<ResourceManager>().getFont(fontName);
		}

		facet.offset = offset;

		facet.color = color;

		facet.position = position;

		auto drawable = single<Renderer>().getDrawable(facet.drawableId);

		drawable.entityId = facet.of();



		Font* font = nullptr;
		if (facet.font != nullptr) {
			font = facet.font;
		}
		else if (_defaultFont != nullptr) {
			font = _defaultFont;
		}
		if (font == nullptr) {
			error("Font not specified for ", facet.of(), ", no default font in TextRenderSystem2d ", this->name());
		}
		if (facet.renderedTextToken == nullptr) {
			auto& rtt = font->getRenderedTextToken();
			rtt->setText(text);
			facet.renderedTextToken = rtt.get();
			_renderedTextTokens.push_back(std::move(rtt));
		}
		else {
			facet.renderedTextToken->setText(text);
		}

		drawable.layerId = _drawableLayerId;
		drawable.camera = getCamera();
		drawable.sourceRect = facet.renderedTextToken->texture()->dimensions();
		drawable.targetRect = SDL_Rect{ position.x + offset.x, position.y + offset.y, drawable.sourceRect.w, drawable.sourceRect.h };		
		drawable.texture = facet.renderedTextToken->texture();


		if (facet.color != Color::get(Color::CommonColor::WHITE)) {
			drawable.textureModulateColor = facet.color;
			drawable.doModulateColor = true;
		}

		drawable.zIndex = position.z + offset.z;
		if (facet.drawableId >= 0) {
			single<Renderer>().updateDrawable(drawable);
		}
		else {
			facet.drawableId = single<Renderer>().createDrawable(drawable);
		}



	}
	TextFacet& TextRenderSystem2d::createTextFacet(Entity& e, std::string fontName, Pixel position, Pixel offset, Color color, std::string text) {


		auto facet = TextFacet{};
		facet.setOf(e);
		
		updateTextFacet(facet, fontName, position, offset, color, text);


		_textFacets.push_back(std::move(facet));
		return _textFacets.back();
	}


	EventProcessor::EventRegistration<TextContentChange> textContentChangeReg{};
	EventProcessor::EventRegistration<FacetDimensionQuery> facetDimensionQueryReg{};

	void TextRenderSystem2d::handleFacetDimensionQuery(FacetDimensionQuery& facetDimensionQuery) {

		for (auto& facet : _textFacets) {
			if (facet.of() == facetDimensionQuery.entity && facet.id() == facetDimensionQuery.facetId) {

				auto d = single<Renderer>().getDrawable(facet.drawableId);

				SDL_Rect dim = d.texture->dimensions();

				facetDimensionQuery.dimensions.setDimension(Dimension{ dim.w, dim.h });
				facetDimensionQuery.found = true;
				return;
			}
		}

	}


	void TextRenderSystem2d::handleTextContentChange(TextContentChange& textContentChange) {

		for (auto& facet : _textFacets)
		if (facet.of() == textContentChange.entity && facet.id() == textContentChange.facetId) {			
			updateTextFacet(facet, textContentChange.font, facet.position, textContentChange.offset.getPixel(), textContentChange.color.getColor(), textContentChange.textContent);
			break;
		}
	}

	void TextRenderSystem2d::destroyFacets(Entity& entity) {

		RenderedTextToken* renderedToken = nullptr;

		for (auto it = std::begin(_textFacets); it != std::end(_textFacets); ++it) {
			if (it->of() == entity) {
				renderedToken = it->renderedTextToken;				
				single<Renderer>().destroyDrawable(it->drawableId);
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

	void TextRenderSystem2d::handleFacetPauseEvent(FacetPauseEvent& pauseEvent) {
		for (auto& facet : _textFacets) {
			if (facet.of() == pauseEvent.entity) {
				if (pauseEvent.facetId == -1 || pauseEvent.facetId == facet.id()) {
					if (pauseEvent.paused) {
						facet.pause();
						single<Renderer>().pauseDrawable(facet.drawableId);
					}
					else {
						facet.resume();
						single<Renderer>().resumeDrawable(facet.drawableId);
					}
					if(pauseEvent.facetId == facet.id()) break;
				}
			}

		}

	}

	void TextRenderSystem2d::setText(TextFacet& facet, std::string text) {
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
		
		auto drawable = single<Renderer>().getDrawable(facet.drawableId);
		drawable.sourceRect = facet.renderedTextToken->texture()->dimensions();
		drawable.targetRect.w = drawable.sourceRect.w;
		drawable.targetRect.h = drawable.sourceRect.h;
		drawable.texture = facet.renderedTextToken->texture();
		single<Renderer>().updateDrawable(drawable);
	}

	void TextRenderSystem2d::setText(Entity& e, std::string text) {

		for (auto& facet : _textFacets) {
			if (facet.of() == e) {

				setText(facet, text);

				return;
			}

		}
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

			auto& newFacet = system->createTextFacet(entity, fontName, position.getPixel(), offset.getPixel(), color.getColor(), text);
			lua.pushStack(newFacet.id());
		}
		else {
			lua.pushStack(-1L);
		}


		return 1;
	}

}