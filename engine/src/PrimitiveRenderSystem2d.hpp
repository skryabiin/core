#ifndef CORE_PRIMITIVE_RENDER_SYSTEM_2D
#define CORE_PRIMITIVE_RENDER_SYSTEM_2D

#include "RenderableSystem2d.hpp"
#include "Drawable.hpp"
#include "PrimitiveFacet.hpp"

namespace core {


	class PrimitiveRenderSystem2d : public RenderableSystem2d {

	public:
		virtual bool createImpl() override;
		virtual bool initializeImpl() override;
		virtual bool resetImpl() override;		
		virtual bool destroyImpl() override;

		bool handleColorChange(ColorChangeEvent& colorChangeEvent);

		virtual void updateDrawablePosition(VisualFacet* vfacet);

		using RenderableSystem2d::handleEvent;

		void createRectangleFacet(Entity& e, Pixel& position, Pixel& offset, Dimension& dimensions, Color& color, bool filled);

		void updateRectangleFacet(Pixel& offset, SDL_Rect& rect, Color& color, bool filled);
		
		virtual void destroyFacets(Entity& e);

		virtual ~PrimitiveRenderSystem2d() {};

	private:

		std::vector<PrimitiveFacet> _facets;		
		
	};





} //end namespace core
#endif