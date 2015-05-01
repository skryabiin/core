#include "PrimitiveRenderSystem2d.hpp"
#include "Renderer.hpp"


namespace core {

	bool PrimitiveRenderSystem2d::initializeImpl() {
		return RenderableSystem2d::initializeImpl();
	}

	bool PrimitiveRenderSystem2d::resetImpl() {
		return RenderableSystem2d::resetImpl();
	}
	bool PrimitiveRenderSystem2d::createImpl() {
		return RenderableSystem2d::createImpl();
	}

	bool PrimitiveRenderSystem2d::destroyImpl() {
		return RenderableSystem2d::destroyImpl();
	}
	

	void PrimitiveRenderSystem2d::handleColorChange(ColorChangeEvent& colorChangeEvent) {
		
		for (auto& facet : _facets) {
			if (facet.of() == colorChangeEvent.entity) {				
				if (colorChangeEvent.facetId == -1 || colorChangeEvent.facetId == facet.of()) {
					facet.drawable.color = colorChangeEvent.color.getColor();
					single<Renderer>().updateDrawable(facet.drawable);
					if (colorChangeEvent.facetId != -1) continue;
				}
			}
		}


	};
		

	void PrimitiveRenderSystem2d::createRectangleFacet(Entity& e, Pixel& offset, SDL_Rect& rect, Color& color, bool filled) {

		auto facet = PrimitiveFacet{};
		facet.setOf(e);
		facet.offset = offset;
		facet.filled = filled;
		facet.dimensions.w = rect.w;
		facet.dimensions.h = rect.h;
		facet.drawable.drawableType = Drawable::DrawableType::RECTANGLE;
		facet.drawable.targetRect = rect;
		facet.drawable.targetRect.x += offset.x;
		facet.drawable.targetRect.y += offset.y;
		facet.drawable.zIndex += offset.z;
		facet.drawable.color = color;
		facet.drawable.id = single<Renderer>().createDrawable(facet.drawable);
		_facets.push_back(std::move(facet));

	}

	



} //end namespace core