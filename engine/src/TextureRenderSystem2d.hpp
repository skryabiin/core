#ifndef CORE_TEXTURE_RENDER_SYSTEM_2D_HPP
#define CORE_TEXTURE_RENDER_SYSTEM_2D_HPP

#include "RenderableSystem2d.hpp"
#include "PositionChangeEvent.hpp"
#include "Font.hpp"
#include "TextureFacet.hpp"
#include "TextureChangeEvent.hpp"
#include "FacetDimensionQuery.hpp"
#include "FacetVector.hpp"

namespace core {



	class TextureRenderSystem2d : public RenderableSystem2d, public EventListener<TextureChangeEvent>, public EventListener<FacetDimensionQuery> {


	public:

		TextureRenderSystem2d();

		using RenderableSystem2d::handleEvent;

		virtual std::vector<Facet*> getFacets(Entity& e) override;

		TextureFacet* createTextureFacet(Entity& e, Pixel position, Pixel offset, Vec2 scale, SDL_Rect source, std::string textureName);

		virtual bool createImpl() override;

		virtual bool initializeImpl() override;

		virtual bool resetImpl() override;

		virtual bool destroyImpl() override;

		virtual void destroyFacets(Entity& entity) override;

		void setDefaultFont(Font* font);

		virtual void updateDrawablePosition(VisualFacet* vfacet);

		bool handleEvent(FacetDimensionQuery& e);

		bool handleEvent(TextureChangeEvent& e);				

		static int createFacet_bind(LuaState& lua);

		virtual ~TextureRenderSystem2d() {};

	private:

		TextureFacet _nullFacet;		

		FacetVector<TextureFacet> _facets;

		std::map<Entity, TextureFacet*> _movingTextures;


	};


} //end namespace core

#endif