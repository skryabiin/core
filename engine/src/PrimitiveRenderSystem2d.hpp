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
		virtual void handleColorChange(ColorChangeEvent& colorChangeEvent);

		void createRectangleFacet(Entity& e, Pixel& offset, SDL_Rect& rect, Color& color, bool filled);

		void updateRectangleFacet(Pixel& offset, SDL_Rect& rect, Color& color, bool filled);
		
		virtual ~PrimitiveRenderSystem2d() {};

	private:

		std::vector<PrimitiveFacet> _facets;		
		
	};





} //end namespace core
#endif