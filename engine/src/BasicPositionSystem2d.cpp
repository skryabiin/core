#include "BasicPositionSystem2d.hpp"
#include "OrientationChangeEvent.hpp"
#include "DimensionChangeEvent.hpp"


namespace core {


	BasicPositionSystem2d::BasicPositionSystem2d() {

		
	}

	bool BasicPositionSystem2d::handleEvent(FacetPauseEvent& pauseEvent) {		
		for (auto& facet : _facets) {
			if (facet.of() == pauseEvent.entity) {
				if (pauseEvent.facetId == -1 || facet.of() == pauseEvent.facetId) {
					(pauseEvent.paused) ? facet.pause() : facet.resume();
					if (pauseEvent.facetId != -1) {
						return false;
					}
				}				
			}
		}
		return true;
	}


	bool BasicPositionSystem2d::createImpl() {		
		return System::createImpl();

	}
	bool BasicPositionSystem2d::initializeImpl() {

		if (!System::initializeImpl()) return false;

	

		return true;
	}


	bool BasicPositionSystem2d::resetImpl() {

		_facets.clear();
		_facets.shrink_to_fit();

		return System::resetImpl();
	}

	bool BasicPositionSystem2d::destroyImpl() {
		return System::destroyImpl();
	}

	void BasicPositionSystem2d::destroyFacets(Entity& e) {
		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {
			if (it->of() == e) {
				it = _facets.erase(it);
				return;
			}
		}
	}

	bool BasicPositionSystem2d::handleEvent(PositionChangeEvent& event) {

		for (auto it = std::begin(_facets); it != std::end(_facets); it++) {

			if (it->of() == event.entity) {
				auto base = (event.relative) ? it->p : Pixel{};
				it->p = event.position.getPixel() + base;				
				return true;
			}
		}
		return true;
	}

	bool BasicPositionSystem2d::handleEvent(DimensionChangeEvent& event) {

		for (auto it = std::begin(_facets); it != std::end(_facets); it++) {

			if (it->of() == event.entity) {
				it->dim = event.dimensions.getDimension();
				return true;
			}
		}
		return true;
	}

	bool BasicPositionSystem2d::handleEvent(OrientationChangeEvent& event) {

		for (auto it = std::begin(_facets); it != std::end(_facets); it++) {

			if (it->of() == event.entity) {
				it->orientation = event.orientation.getVec2();
				return true;
			}
		}
		return true;
	}


	bool BasicPositionSystem2d::handleEvent(EntitiesInRectQuery& query) {

		int x1 = query.rect[0];
		int y1 = query.rect[1];
		int x2 = query.rect[2];
		int y2 = query.rect[3];

		int a1, b1, a2, b2;
		bool collided;
		for (PositionFacet& facet : _facets) {

			if (facet.isPaused()) continue;
			a1 = facet.p.x;
			b1 = facet.p.y;
			a2 = a1 + facet.dim.w;
			b2 = b1 + facet.dim.h;
			collided = ((inRangeSorted(min(x1, x2), max(x1, x2), a1, a2) & inRangeSorted(min(y1, y2), max(y1, y2), b1, b2)) == 1);
			if (collided) {
				int id = facet.of();
				query.entities.push_back(id);
			}

		}
		return true;

	}


	bool BasicPositionSystem2d::handleEvent(EntityPositionQuery& query) {


		//return if another system already has the position
		if (query.found) return false;

		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {
			if (it->of() == query.entity) {
				query.position.setPixel(it->p);
				query.orientation.setVec2(it->orientation);
				query.dimensions.setDimension(it->dim);
				query.found = true;
				break;
			}
		}

		return !query.found;
	}

	PositionFacet& BasicPositionSystem2d::createFacet(Entity& e, Pixel position, Dimension dimensions, Vec2 orientation) {

		auto facet = PositionFacet{};
		facet.setOf(e);
		facet.p = position;
		facet.dim = dimensions;
		facet.orientation = orientation;
		_facets.push_back(facet);
		return _facets.back();

	}

	int BasicPositionSystem2d::createFacet_bind(LuaState& lua) {

		auto systemName = lua.pullStack<std::string>(1);
		auto entity = lua.pullStack<int>(2);

		auto system = single<Core>().getSystemByName<BasicPositionSystem2d>(systemName);

		LuaPixel position = lua["position"];
		LuaDimension dimensions = lua["dimensions"];
		LuaVec2 orientation = lua["orientation"];
		auto& newFacet = system->createFacet(entity, position.getPixel(), dimensions.getDimension(), orientation.getVec2());			
		lua.pushStack(newFacet.id());

		return 1;
	}

}