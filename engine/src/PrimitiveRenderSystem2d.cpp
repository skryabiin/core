#include "PrimitiveRenderSystem2d.hpp"
#include "Renderer.hpp"
#include "EntityPositionQuery.hpp"

namespace core {

	bool PrimitiveRenderSystem2d::initializeImpl() {
		return RenderableSystem2d::initializeImpl();
	}

	bool PrimitiveRenderSystem2d::resetImpl() {

		auto dc = DrawableChange{};
		dc.layerId = _drawableLayerId;
		dc.operation = DrawableChange::Operation::DESTROY_DRAWABLE; 
		
		for (auto& facet : _facets) {			
			dc.facetId = facet.id();			
			single<Renderer>().applyDrawableChange(dc);
		}


		return RenderableSystem2d::resetImpl();
	}
	bool PrimitiveRenderSystem2d::createImpl() {
		return RenderableSystem2d::createImpl();
	}

	bool PrimitiveRenderSystem2d::destroyImpl() {
		return RenderableSystem2d::destroyImpl();
	}
	

	bool PrimitiveRenderSystem2d::handleColorChange(ColorChangeEvent& colorChangeEvent) {
		
		for (auto& facet : _facets) {
			if (facet.of() == colorChangeEvent.entity) {				
				if (colorChangeEvent.facetId == -1 || colorChangeEvent.facetId == facet.id()) {
					auto dc = DrawableChange{};
					dc.operation = DrawableChange::Operation::CHANGE_COLOR;
					dc.facetId = facet.id();
					dc.layerId = _drawableLayerId;
					dc.color = colorChangeEvent.color.getColor();
					single<Renderer>().applyDrawableChange(dc);
					if (colorChangeEvent.facetId != -1) return false;
				}
			}
		}
		return true;
	}

	

	void PrimitiveRenderSystem2d::updateDrawablePosition(VisualFacet* vfacet) {

		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::CHANGE_PRIMITIVE_POSITION;
		dc.facetId = vfacet->id();
		dc.layerId = _drawableLayerId;
		auto& position = vfacet->position;
		auto& offset = vfacet->offset;
		auto& dimensions = vfacet->dimensions;
		auto& scale = vfacet->scale;
		dc.zIndex = position.z + offset.z;

		dc.vertices.push_back(Point( position.x + offset.x, position.y + offset.y ));
		dc.vertices.push_back(Point( position.x + offset.x + dimensions.w * scale.x, position.y + offset.y ));
		dc.vertices.push_back(Point( position.x + offset.x + dimensions.w * scale.x, position.y + offset.y + dimensions.h * scale.y));
		dc.vertices.push_back(Point( position.x + offset.x, position.y + offset.y + dimensions.h * scale.y));

		single<Renderer>().applyDrawableChange(dc);
	}

	void PrimitiveRenderSystem2d::createRectangleFacet(Entity& e, Pixel& position, Pixel& offset, Dimension& dimensions, Color& color, bool filled) {

		auto facet = PrimitiveFacet{};
		facet.setOf(e);
		facet.offset = offset;		
		facet.filled = filled;
		facet.dimensions = dimensions;				
		facet.color = color;

		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::CREATE_PRIMITIVE_DRAWABLE;
		dc.facetId = facet.id();
		dc.layerId = _drawableLayerId;
		dc.filled = filled;
		dc.zIndex = position.z + offset.z;
		dc.color = color;
		

		
		dc.vertices.push_back(Pixel{ position.x + offset.x, position.y + offset.y });
		dc.vertices.push_back(Pixel{ position.x + offset.x + dimensions.w, position.y + offset.y });
		dc.vertices.push_back(Pixel{ position.x + offset.x + dimensions.w, position.y + offset.y + dimensions.h });
		dc.vertices.push_back(Pixel{ position.x + offset.x, position.y + offset.y + dimensions.h });
		dc.camera = _camera;
		dc.shaderProgramName = "shape2d";
		single<Renderer>().applyDrawableChange(dc);
		_facets.push_back(std::move(facet));

	}

	void PrimitiveRenderSystem2d::destroyFacets(Entity& e) {
		auto it = std::begin(_facets);
		auto endIt = std::end(_facets);

		while (it != endIt) {		
			if (it->of() == e) {				
				it = _facets.erase(it);
				endIt = std::end(_facets);
			}
			else {
				++it;
			}
		}
	}



} //end namespace core