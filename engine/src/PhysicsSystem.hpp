#ifndef CORE_PHYSICS_FACET_H
#define CORE_PHYSICS_FACET_H

#include <vector>
#include <string>
#include "LuaUtil.hpp"
#include "Util.hpp"
#include "Facet.hpp"
#include "PositionFacet.hpp"
#include "Geometry.hpp"
#include "UpdateableSystem.hpp"
#include "PhysicsFacet.hpp"
#include "AccelerationChangeEvent.hpp"
#include "VelocityChangeEvent.hpp"
#include "PositionChangeEvent.hpp"
#include "ObjectCollisionQuery.hpp"
#include "RuntimeContext.hpp"
#include "EntityPositionQuery.hpp"
#include "EntitiesInRectQuery.hpp"

namespace core {


	




	struct Contact {

		Contact() : collided{ false } {
			adjA[0] = 0;
			adjA[1] = 0;
			adjB[0] = 0;
			adjB[1] = 0;
			subT[0] = 0;
			subT[1] = 0;
		};


		//x values before and after move
		int bx[4];

		//y values before and after move
		int by[4];

		//if this collided with the first
		bool collided;

		//the fraction of time it took to reach each axis
		float subT[2];

		//the necessary adjustments to the first body 
		//for each axis
		int adjA[2];

		//the necessary adjustments to the second body 
		//for each axis
		int adjB[2];
		PhysicsFacet* b;
	};

	//a combination of all bodies that the test body
	//has run into.  for blocking bodies, virtual contacts may be
	//created
	struct Collision {

		Collision() {
			finalAdjA[0] = 0;
			finalAdjA[1] = 0;
			subT[0] = 2.0f;
			subT[1] = 2.0f;
		}
		int ax[4];
		int ay[4];
		int finalAdjA[2];
		float subT[2];
		PhysicsFacet* a;
		std::vector<Contact> contacts;
		static const int typeId = 3;
	};


	

	class PhysicsSystem : public UpdateableSystem {

	public:

		PhysicsSystem();

		virtual InitStatus initializeImpl() override;

		virtual InitStatus resetImpl() override;

		void updateImpl(RuntimeContext& context);

		PhysicsFacet& createPhysicsFacet(Entity& e, bool fixed);

		virtual std::vector<Facet*> getFacets(Entity& e);

		virtual void destroyFacets(Entity& entity) override;

		void setGravity(Vec2& gravity);

		Vec2 gravity() const;

		void checkSingleContact(Collision& col, Contact& c);

		void solveCollision(Collision& col);

		void handleVelocityChange(VelocityChangeEvent& event);

		void handleAccelerationChange(AccelerationChangeEvent& event);

		void handlePositionChange(PositionChangeEvent& event);

		void answerObjectCollisionQuery(ObjectCollisionQuery& query);

		void answerEntityPositionQuery(EntityPositionQuery& query);

		void answerEntitiesInRectQuery(EntitiesInRectQuery& query);

		virtual void handleFacetPauseEvent(FacetPauseEvent& pauseEvent) override;

		void setPosition(Entity& e, Pixel p);

		static int createFacet_bind(LuaState& lua);
	private:

		void PhysicsSystem::getPositionValues(PhysicsFacet& pf, int(&x)[4], int(&y)[4]);

		std::vector<PhysicsFacet> _physicsFacets;
		
		std::vector<PhysicsFacet> _fixedPhysicsFacets;

		EventFilter<VelocityChangeEvent> _velocityChangeFilter;

		EventFilter<AccelerationChangeEvent> _accelerationChangeFilter;

		EventFilter<ObjectCollisionQuery> _objectCollisionFilter;

		EventFilter<PositionChangeEvent> _positionChangeFilter;

		EventFilter<EntityPositionQuery> _positionQueryFilter;

		EventFilter<EntitiesInRectQuery> _entitiesInRectQueryFilter;

		Vec2 _gravity;


	};

} //endif

#endif