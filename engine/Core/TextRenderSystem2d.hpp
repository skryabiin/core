#ifndef CORE_TEXT_RENDER_SYSTEM_2D_HPP
#define CORE_TEXT_RENDER_SYSTEM_2D_HPP

#include "RenderableSystem2d.hpp"
#include "TextFacet.hpp"
#include "EventFilter.hpp"
#include "TextContentChange.hpp"
#include "Entity.hpp"
#include "FacetDimensionQuery.hpp"

namespace core {


	class TextRenderSystem2d : public RenderableSystem2d {

	public:

		TextRenderSystem2d();

		virtual void updateDrawPosition(PositionChangeEvent& positionChangeEvent) override;

		virtual std::vector<Facet*> getFacets(Entity& e) override;

		void updateTextFacet(TextFacet& facet, std::string fontName, Pixel position, Pixel offset, Color color, std::string text);

		TextFacet& createTextFacet(Entity& e, std::string fontName, Pixel position, Pixel offset, Color color, std::string text);

		void handleTextContentChange(TextContentChange& textContentChange);		
		
		void handleFacetDimensionQuery(FacetDimensionQuery& facetDimensionQuery);

		virtual InitStatus initializeImpl() override;

		virtual InitStatus resetImpl() override;		

		virtual void destroyFacets(Entity& entity) override;

		void setText(Entity& e, std::string text);

		void setText(TextFacet& facet, std::string text);

		void setDefaultFont(Font* defaultFont);

		virtual void handleFacetPauseEvent(FacetPauseEvent& pauseEvent) override;

		virtual void renderFacet(SDL_Renderer* renderer, Facet* facet) {};

		static int createFacet_bind(LuaState& lua);

	private:

		std::vector<TextFacet> _textFacets;

		std::vector<std::unique_ptr<RenderedTextToken>> _renderedTextTokens;

		EventFilter<TextContentChange> _textContentChangeFilter;

		EventFilter<FacetDimensionQuery> _facetDimensionQueryFilter;

		Font* _defaultFont;

	};

} //end namespace core

#endif