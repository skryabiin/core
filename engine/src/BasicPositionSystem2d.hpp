#ifndef CORE_BASIC_POSITION_SYSTEM_2D_HPP
#define CORE_BASIC_POSITION_SYSTEM_2D_HPP

#include "System.hpp"
#include "PositionFacet.hpp"
#include "PositionChangeEvent.hpp"
#include "DimensionChangeEvent.hpp"
#include "OrientationChangeEvent.hpp"
#include "EntityPositionQuery.hpp"
#include "EntitiesInRectQuery.hpp"

namespace core {

	class BasicPositionSystem2d : public System, public EventListener<PositionChangeEvent>, public EventListener<DimensionChangeEvent>, public EventListener<OrientationChangeEvent>,
		public EventListener<EntityPositionQuery>, public EventListener<EntitiesInRectQuery> {

	public:

		BasicPositionSystem2d();

		bool handleEvent(PositionChangeEvent& event);

		bool handleEvent(DimensionChangeEvent& event);

		bool handleEvent(OrientationChangeEvent& event);

		bool handleEvent(EntityPositionQuery& query);

		bool handleEvent(EntitiesInRectQuery& query);

		bool handleEvent(FacetPauseEvent& pauseEvent);

		PositionFacet& createFacet(Entity& e, Pixel position, Dimension dimensions, Vec2 orientation);

		virtual ~BasicPositionSystem2d() {};
		

		virtual bool createImpl() override;

		virtual bool initializeImpl() override;

		virtual bool resetImpl() override;

		virtual bool destroyImpl() override; 

		virtual void destroyFacets(Entity& e) override;

		static int createFacet_bind(LuaState& lua);

	private:

		std::vector<PositionFacet> _facets;

	};
} //end namespace core

#endif