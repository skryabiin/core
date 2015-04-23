#ifndef CORE_TEXTURE_RENDER_SYSTEM_2D_HPP
#define CORE_TEXTURE_RENDER_SYSTEM_2D_HPP

#include "RenderableSystem2d.hpp"
#include "PositionChangeEvent.hpp"
#include "Font.hpp"
#include "TextureFacet.hpp"
#include "TextureChangeEvent.hpp"


namespace core {



	class TextureRenderSystem2d : public RenderableSystem2d {


	public:

		TextureRenderSystem2d();

		virtual void updateDrawPosition(PositionChangeEvent& positionChange) override;

		virtual std::vector<Facet*> getFacets(Entity& e) override;

		TextureFacet& createTextureFacet(Entity& e, Pixel position, Pixel offset, Dimension dimensions, Vec2 scale, SDL_Rect source, std::string textureName);

		virtual InitStatus initializeImpl() override;

		virtual InitStatus resetImpl() override;

		virtual void destroyFacets(Entity& entity) override;

		void setDefaultFont(Font* font);

		void handleTextureChange(TextureChangeEvent& e);

		TextureFacet& updateTexture(TextureChangeEvent& e);

		virtual void handleFacetPauseEvent(FacetPauseEvent& pauseEvent) override;

		virtual void handleEntityLayerQuery(EntityLayerQuery& entityLayerQuery) override;

		virtual void updateColor(ColorChangeEvent& colorChangeEvent) override;

		static int createFacet_bind(LuaState& lua);

	private:

		TextureFacet _nullFacet;

		std::vector<TextureFacet> _textureFacets;

		EventFilter<TextureChangeEvent> _textureChangeFilter;


		std::map<Entity, TextureFacet*> _movingTextures;


	};


} //end namespace core

#endif