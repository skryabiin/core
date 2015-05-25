#include <functional>
#include "Renderer.hpp"
#include "RenderedTextToken.hpp"
#include "TextRenderSystem2d.hpp"
#include "Drawable.hpp"
#include "TextContentChangeEvent.hpp"
#include "Core.hpp"
#include "OffsetChangeEvent.hpp"
#include "Interface.hpp"
#include "World.hpp"

namespace core {




	TextRenderSystem2d::TextRenderSystem2d() {


	}

	bool TextRenderSystem2d::createImpl() {
		if (_facets.create(20) != InitStatus::CREATE_TRUE) return false;
		return RenderableSystem2d::createImpl();
	}

	bool TextRenderSystem2d::initializeImpl() {
	
		if (_facets.initialize(2) != InitStatus::INIT_TRUE) return false;
		if (!RenderableSystem2d::initializeImpl()) return false;
		_camera = single<World>().camera();
		return true;
	}

	bool TextRenderSystem2d::resetImpl() {

		auto dc = DrawableChange{};
		dc.layerId = _drawableLayerId;
		dc.operation = DrawableChange::Operation::DESTROY_DRAWABLE;
		auto deleteTextTokens = [&dc](TextFacet* facet)->void{
			dc.facetId = facet->id();
			single<Renderer>().applyDrawableChange(dc);
			delete facet->renderedTextToken;
		};

		bool facetVectorDestroyStatus = (_facets.reset() == InitStatus::CREATE_TRUE);
		
		return RenderableSystem2d::resetImpl() && facetVectorDestroyStatus;
	}

	bool TextRenderSystem2d::destroyImpl() {
		bool facetVectorDestroyStatus = (_facets.destroy() == InitStatus::CREATE_FALSE);		
		return RenderableSystem2d::destroyImpl() && facetVectorDestroyStatus;
	}
	

	void TextRenderSystem2d::updateDrawablePosition(VisualFacet* vfacet) {
		auto& offset = vfacet->offset;
		auto& position = vfacet->position;
		auto& dimensions = vfacet->dimensions;
		auto& scale = vfacet->scale;
		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::CHANGE_TEXTURE_POSITION;
		dc.facetId = vfacet->id();
		dc.layerId = _drawableLayerId;
		dc.zIndex = vfacet->position.z + offset.z;
		dc.targetRect.x = roundFloat(position.x + offset.x);
		dc.targetRect.y = roundFloat(position.y + offset.y);
		dc.targetRect.w = dimensions.w * scale.x;
		dc.targetRect.h = dimensions.h * scale.y;

		single<Renderer>().applyDrawableChange(dc);

	}



	bool TextRenderSystem2d::handleEvent(ColorChangeEvent& colorChangeEvent) {
		return true;
	}



	std::vector<Facet*> TextRenderSystem2d::getFacets(Entity& e) {		
		return _facets.getBaseFacets(e);
	}

	
	TextFacet* TextRenderSystem2d::createTextFacet(Entity& e, std::string fontName, Pixel position, Pixel offset, Vec2 scale, Color color, std::string text) {


		auto facet = TextFacet{};
		facet.setOf(e);		
		facet.offset = offset;
		facet.color = color;
		facet.scale = scale;
		facet.position = position;		
		facet.textContent = text;				
		facet.font = (!fontName.compare("default")) ? single<TextureManager>().getDefaultFont() : single<TextureManager>().getFont(fontName);

		facet.renderedTextToken= facet.font->getRenderedTextToken();
		facet.renderedTextToken->setText(text);
		facet.dimensions.w = facet.renderedTextToken->texture()->dimensions().w;
		facet.dimensions.h = facet.renderedTextToken->texture()->dimensions().h;
		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::CREATE_TEXTURE_DRAWABLE;
		dc.facetId = facet.id();
		dc.layerId = _drawableLayerId;
		dc.zIndex = position.z + offset.z;
		dc.camera = _camera;
		dc.texture = facet.renderedTextToken->texture();
		dc.textureCoordinates = dc.texture->dimensions();
		dc.targetRect.x = position.x + offset.x;
		dc.targetRect.y = position.y + offset.y;
		dc.targetRect.w = roundFloat(dc.textureCoordinates.w * scale.x);
		dc.targetRect.h = roundFloat(dc.textureCoordinates.h * scale.y);
		dc.shaderProgramName = "textureRender2d";
		single<Renderer>().applyDrawableChange(dc);

		return _facets.addFacet(facet);
	}


	bool TextRenderSystem2d::handleEvent(FacetDimensionQuery& facetDimensionQuery) {
		long& facetId = facetDimensionQuery.facetId;
		auto facet = _facets[facetId];
		if (facet == nullptr) {
			return true;
		}
		else {
			facetDimensionQuery.dimensions.setDimension(Dimension(facet->dimensions.w * facet->scale.x, facet->dimensions.h * facet->scale.y));
			facetDimensionQuery.found = true;
			return false;
		}		
	}


	bool TextRenderSystem2d::handleEvent(TextContentChangeEvent& textContentChange) {

		int& facetId = textContentChange.facetId;
		auto facet = _facets[facetId];
		if (facet == nullptr) {
			return true;
		} else {
		 	setText(facet, textContentChange.font, textContentChange.textContent);
			return false;			
		}		
	}

	void TextRenderSystem2d::destroyFacets(Entity& entity) {

		RenderedTextToken* renderedToken = nullptr;

		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::DESTROY_DRAWABLE;
		dc.layerId = _drawableLayerId;		

		_facets.forEach([&entity, &dc](TextFacet* facet)->void{
			if (facet->of() == entity) {
				delete facet->renderedTextToken;
				dc.facetId = facet->id();
				single<Renderer>().applyDrawableChange(dc);
			}
		});
		
		_facets.removeFacets(entity);
	}

	//TODO do i need the rendered text token generation?
	void TextRenderSystem2d::setText(TextFacet* facet, std::string font ,std::string text) {
		facet->textContent = text;

		if (facet->renderedTextToken == nullptr) {
			Font* font = nullptr;
			if (facet->font != nullptr) {
				font = facet->font;
			}
			else if (_defaultFont != nullptr) {
				font = _defaultFont;
			}
			if (font != nullptr) {
				
				facet->renderedTextToken = font->getRenderedTextToken();
				facet->renderedTextToken->setText(text);				
			}
		}
		else {
			facet->renderedTextToken->setText(text);
		}

		
		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::CHANGE_TEXTURE;
		dc.facetId = facet->id();
		dc.layerId = _drawableLayerId;
		dc.texture = facet->renderedTextToken->texture();
		dc.textureCoordinates = facet->renderedTextToken->texture()->dimensions();
		facet->dimensions.w = dc.textureCoordinates.w;
		facet->dimensions.h = dc.textureCoordinates.h;
		single<Renderer>().applyDrawableChange(dc);
		updateDrawablePosition(facet);
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

			auto newFacet = system->createTextFacet(entity, fontName,position.getPixel(), offset.getPixel(), scale.getVec2(), color.getColor(), text);
			lua.pushStack(newFacet->id());
		}
		else {
			lua.pushStack(-1L);
		}


		return 1;
	}

}