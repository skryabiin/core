#include "BasicPositionSystem2d.hpp"
#include "OrientationChangeEvent.hpp"
#include "DimensionChangeEvent.hpp"


namespace core {


	BasicPositionSystem2d::BasicPositionSystem2d() {

		
	}

	void BasicPositionSystem2d::handleFacetPauseEvent(FacetPauseEvent& pauseEvent) {		
		for (auto& facet : _facets) {
			if (facet.of() == pauseEvent.entity) {
				if (pauseEvent.facetId == -1 || facet.of() == pauseEvent.facetId) {
					(pauseEvent.paused) ? facet.pause() : facet.resume();
				}
				break;
			}
		}

	}


	InitStatus BasicPositionSystem2d::initializeImpl() {

		std::function<void(BasicPositionSystem2d*, PositionChangeEvent&)> positionChangeCallback = std::mem_fn(&BasicPositionSystem2d::handlePositionChange);
		_positionChangeFilter.init(this, positionChangeCallback);
		single<EventProcessor>().addFilter(&_positionChangeFilter);

		std::function<void(BasicPositionSystem2d*, OrientationChangeEvent&)> orientationChangeCallback = std::mem_fn(&BasicPositionSystem2d::handleOrientationChange);
		_orientationChangeFilter.init(this, orientationChangeCallback);
		single<EventProcessor>().addFilter(&_orientationChangeFilter);

		std::function<void(BasicPositionSystem2d*, DimensionChangeEvent&)> dimensionChangeCallback = std::mem_fn(&BasicPositionSystem2d::handleDimensionChange);
		_dimensionChangeFilter.init(this, dimensionChangeCallback);
		single<EventProcessor>().addFilter(&_dimensionChangeFilter);

		std::function<void(BasicPositionSystem2d*, EntityPositionQuery&)> positionQueryCallback = std::mem_fn(&BasicPositionSystem2d::answerEntityPositionQuery);
		_positionQueryFilter.init(this, positionQueryCallback);
		single<EventProcessor>().addFilter(&_positionQueryFilter);

		std::function<void(BasicPositionSystem2d*, EntitiesInRectQuery&)> entitiesInRectQueryCallback = std::mem_fn(&BasicPositionSystem2d::answerEntitiesInRectQuery);
		_entitiesInRectQueryFilter.init(this, entitiesInRectQueryCallback);
		single<EventProcessor>().addFilter(&_entitiesInRectQueryFilter);

		return System::initializeImpl();
	}


	InitStatus BasicPositionSystem2d::resetImpl() {

		single<EventProcessor>().removeFilter(&_positionChangeFilter);
		single<EventProcessor>().removeFilter(&_orientationChangeFilter);
		single<EventProcessor>().removeFilter(&_dimensionChangeFilter);
		single<EventProcessor>().removeFilter(&_positionQueryFilter);
		single<EventProcessor>().removeFilter(&_entitiesInRectQueryFilter);

		_facets.clear();
		_facets.shrink_to_fit();

		return System::resetImpl();
	}

	void BasicPositionSystem2d::destroyFacets(Entity& e) {
		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {
			if (it->of() == e) {
				it = _facets.erase(it);
				return;
			}
		}
	}

	void BasicPositionSystem2d::handlePositionChange(PositionChangeEvent& event) {		

		for (auto it = std::begin(_facets); it != std::end(_facets); it++) {

			if (it->of() == event.entity) {
				auto base = (event.relative) ? it->p : Pixel{};
				it->p = event.position.getPixel() + base;				
				return;
			}
		}
	}

	void BasicPositionSystem2d::handleDimensionChange(DimensionChangeEvent& event) {

		for (auto it = std::begin(_facets); it != std::end(_facets); it++) {

			if (it->of() == event.entity) {
				it->dim = event.dimensions.getDimension();
				return;
			}
		}
	}

	void BasicPositionSystem2d::handleOrientationChange(OrientationChangeEvent& event) {

		for (auto it = std::begin(_facets); it != std::end(_facets); it++) {

			if (it->of() == event.entity) {
				it->orientation = event.orientation.getVec2();
				return;
			}
		}
	}


	void BasicPositionSystem2d::answerEntitiesInRectQuery(EntitiesInRectQuery& query) {

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


	}


	void BasicPositionSystem2d::answerEntityPositionQuery(EntityPositionQuery& query) {


		//return if another system already has the position
		if (query.found) return;

		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {
			if (it->of() == query.entity) {
				query.position.setPixel(it->p);
				query.orientation.setVec2(it->orientation);
				query.dimensions.setDimension(it->dim);
				query.found = true;
				break;
			}
		}
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

	EventProcessor::EventRegistration<OrientationChangeEvent> orientationChangeEventReg{};
	EventProcessor::EventRegistration<DimensionChangeEvent> dimensionChangeEventReg{};
}