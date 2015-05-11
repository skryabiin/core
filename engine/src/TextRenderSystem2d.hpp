#ifndef CORE_TEXT_RENDER_SYSTEM_2D_HPP
#define CORE_TEXT_RENDER_SYSTEM_2D_HPP

#include "RenderableSystem2d.hpp"
#include "TextFacet.hpp"
#include "TextContentChangeEvent.hpp"
#include "Entity.hpp"
#include "FacetDimensionQuery.hpp"

namespace core {


	class TextRenderSystem2d : public RenderableSystem2d, public EventListener<ColorChangeEvent>, public EventListener<TextContentChangeEvent>, public EventListener<FacetDimensionQuery> {

	public:

		TextRenderSystem2d();
		

		virtual std::vector<Facet*> getFacets(Entity& e) override;

		//void updateTextFacet(TextFacet& facet, std::string fontName, Pixel position, Pixel offset, Color color, std::string text);

		TextFacet& createTextFacet(Entity& e, std::string fontName,  Pixel position, Pixel offset, Vec2 scale, Color color, std::string text);

		using RenderableSystem2d::handleEvent;

		virtual void updateDrawablePosition(VisualFacet* vfacet);

		bool handleEvent(ColorChangeEvent& colorChangeEvent);

		bool handleEvent(TextContentChangeEvent& textContentChange);
		
		bool handleEvent(FacetDimensionQuery& facetDimensionQuery);

		virtual bool createImpl() override;

		virtual bool initializeImpl() override;

		virtual bool resetImpl() override;	

		virtual bool destroyImpl() override;

		virtual void destroyFacets(Entity& entity) override;

		void setText(TextFacet& facet, std::string text, std::string font);

		void setDefaultFont(Font* defaultFont);


		virtual void renderFacet(SDL_Renderer* renderer, Facet* facet) {};

		static int createFacet_bind(LuaState& lua);

		virtual ~TextRenderSystem2d() {};
	private:

		std::vector<TextFacet> _textFacets;

		std::vector<std::unique_ptr<RenderedTextToken>> _renderedTextTokens;

		Font* _defaultFont;

	};

} //end namespace core

#endif