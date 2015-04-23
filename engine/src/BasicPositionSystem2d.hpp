#ifndef CORE_BASIC_POSITION_SYSTEM_2D_HPP
#define CORE_BASIC_POSITION_SYSTEM_2D_HPP

#include "System.hpp"
#include "PositionFacet.hpp"
#include "EventFilter.hpp"
#include "PositionChangeEvent.hpp"
#include "DimensionChangeEvent.hpp"
#include "OrientationChangeEvent.hpp"
#include "EntityPositionQuery.hpp"
#include "EntitiesInRectQuery.hpp"

namespace core {

	class BasicPositionSystem2d : public System {

	public:

		BasicPositionSystem2d();

		void handlePositionChange(PositionChangeEvent& event);

		void handleDimensionChange(DimensionChangeEvent& event);

		void handleOrientationChange(OrientationChangeEvent& event);

		void answerEntityPositionQuery(EntityPositionQuery& query);

		void answerEntitiesInRectQuery(EntitiesInRectQuery& query);

		PositionFacet& createFacet(Entity& e, Pixel position, Dimension dimensions, Vec2 orientation);

		virtual void handleFacetPauseEvent(FacetPauseEvent& pauseEvent) override;

		virtual InitStatus initializeImpl() override;

		virtual InitStatus resetImpl() override;

		virtual void destroyFacets(Entity& e) override;

		static int createFacet_bind(LuaState& lua);

	private:

		std::vector<PositionFacet> _facets;

		EventFilter<PositionChangeEvent> _positionChangeFilter;

		EventFilter<OrientationChangeEvent> _orientationChangeFilter;

		EventFilter<DimensionChangeEvent> _dimensionChangeFilter;

		EventFilter<EntityPositionQuery> _positionQueryFilter;

		EventFilter<EntitiesInRectQuery> _entitiesInRectQueryFilter;

	};
} //end namespace core

#endif