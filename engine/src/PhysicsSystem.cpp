
#include <algorithm>
#include "lua.hpp"
#include "PhysicsSystem.hpp"
#include "Entity.hpp"
#include "Facet.hpp"
#include "Event.hpp"
#include "Math.hpp"
#include "Console.hpp"
#include "Core.hpp"
#include "CollisionState.hpp"



namespace core {






	PhysicsSystem::PhysicsSystem() :  _gravity{ 0.0f, 0.0f } {
		
		std::function<void(PhysicsSystem*, VelocityChangeEvent&)> velocityChangeCallback = std::mem_fn(&PhysicsSystem::handleVelocityChange);
		_velocityChangeFilter.init(this, velocityChangeCallback);
		single<EventProcessor>().addFilter(&_velocityChangeFilter);


		std::function<void(PhysicsSystem*, AccelerationChangeEvent&)> accelerationChangeCallback = std::mem_fn(&PhysicsSystem::handleAccelerationChange);
		_accelerationChangeFilter.init(this, accelerationChangeCallback);
		single<EventProcessor>().addFilter(&_accelerationChangeFilter);

		std::function<void(PhysicsSystem*, ObjectCollisionQuery&)> objectCollisionCallback = std::mem_fn(&PhysicsSystem::answerObjectCollisionQuery);
		_objectCollisionFilter.init(this, objectCollisionCallback);
		single<EventProcessor>().addFilter(&_objectCollisionFilter);

		std::function<void(PhysicsSystem*, PositionChangeEvent&)> positionChangeCallback = std::mem_fn(&PhysicsSystem::handlePositionChange);
		_positionChangeFilter.init(this, positionChangeCallback);
		single<EventProcessor>().addFilter(&_positionChangeFilter);


		std::function<void(PhysicsSystem*, EntityPositionQuery&)> positionQueryCallback = std::mem_fn(&PhysicsSystem::answerEntityPositionQuery);
		_positionQueryFilter.init(this, positionQueryCallback);
		single<EventProcessor>().addFilter(&_positionQueryFilter);

		std::function<void(PhysicsSystem*, EntitiesInRectQuery&)> entitiesInRectQueryCallback = std::mem_fn(&PhysicsSystem::answerEntitiesInRectQuery);
		_entitiesInRectQueryFilter.init(this, entitiesInRectQueryCallback);
		single<EventProcessor>().addFilter(&_positionQueryFilter);

	}




	PhysicsFacet& PhysicsSystem::createPhysicsFacet(Entity& e, bool fixed) {

		if (fixed) {
			_fixedPhysicsFacets.emplace_back();
			auto& newFacet = _fixedPhysicsFacets.back();
			newFacet.setOf(e);
			return newFacet;			
		}
		else {
			_physicsFacets.emplace_back();
			auto& newFacet = _physicsFacets.back();
			newFacet.setOf(e);
			return newFacet;			
		}
	}

	void PhysicsSystem::handleFacetPauseEvent(FacetPauseEvent& pauseEvent) {

	}

	std::vector<Facet*> PhysicsSystem::getFacets(Entity& e) {

		auto out = std::vector<Facet*>{};


		for (auto& facet : _physicsFacets) {
			if (facet.of() == e) {
				out.push_back(&facet);
			}
		}

		for (auto& facet : _fixedPhysicsFacets) {
			if (facet.of() == e) {
				out.push_back(&facet);
			}
		}

		return out;
	}


	void PhysicsSystem::handleVelocityChange(VelocityChangeEvent& event) {

		for (unsigned i = 0; i < _physicsFacets.size(); i++) {
			if (_physicsFacets[i].of() == event.entity) {

				Vec2 newVel = _physicsFacets[i].velocity;

				if (event.axes <= 0) {
					newVel.x = (event.additive) ? newVel.x + event.velocityChange[0] : event.velocityChange[0];
				}
				if (event.axes >= 0) {
					newVel.y = (event.additive) ? newVel.y + event.velocityChange[1] : event.velocityChange[1];
				}


				_physicsFacets[i].velocity = newVel;
				break;
			}
		}
	}

	void PhysicsSystem::handleAccelerationChange(AccelerationChangeEvent& event) {
		for (unsigned i = 0; i < _physicsFacets.size(); i++) {
			if (_physicsFacets[i].of() == event.entity) {
				Vec2 newAccel = _physicsFacets[i].acceleration;

				if (event.axes <= 0) {
					newAccel.x = (event.additive) ? newAccel.x + event.accelerationChange[0] : event.accelerationChange[0];
				}
				if (event.axes >= 0) {
					newAccel.y = (event.additive) ? newAccel.y + event.accelerationChange[1] : event.accelerationChange[1];
				}


				_physicsFacets[i].acceleration = newAccel;
				break;
			}
		}

	}

	//modded to allow explicit position changes of fixed entities
	void PhysicsSystem::handlePositionChange(PositionChangeEvent& event) {
		if (event.originator() != nullptr && *event.originator() == *this) return;
		
		for (auto it = std::begin(_physicsFacets); it != std::end(_physicsFacets); it++) {

			if (it->of() == event.entity) {
				auto base = (event.relative) ? it->p : Pixel{};
				it->p = event.position.getPixel() + base;
				return;
			}
		}

		for (auto it = std::begin(_fixedPhysicsFacets); it != std::end(_fixedPhysicsFacets); ++it) {
			if (it->of() == event.entity) {
				auto base = (event.relative) ? it->p : Pixel{};
				it->p = event.position.getPixel() + base;
				return;
			}

		}

	}


	void PhysicsSystem::answerEntityPositionQuery(EntityPositionQuery& query) {
		
		
		//return if another system already has the position
		if (query.found) return;
		
		for (auto it = std::begin(_physicsFacets); it != std::end(_physicsFacets); ++it) {
			if (it->of() == query.entity) {
				query.position.setPixel(it->p);
				query.orientation.setVec2(it->orientation);
				query.dimensions.setDimension(it->dim);
				query.found = true;
				break;
			}
		}
		if (query.found) return;

		for (auto it = std::begin(_fixedPhysicsFacets); it != std::end(_fixedPhysicsFacets); ++it) {
			if (it->of() == query.entity) {
				query.position.setPixel(it->p);
				query.orientation.setVec2(it->orientation);
				query.dimensions.setDimension(it->dim);
				query.found = true;
				break;
			}
		
		}		

	}

	void PhysicsSystem::setGravity(Vec2& gravity) {

		_gravity = gravity;
	}

	Vec2 PhysicsSystem::gravity() const {
		return _gravity;
	}

	InitStatus PhysicsSystem::initializeImpl() {

		std::function<void(PhysicsSystem*, VelocityChangeEvent&)> velocityChangeCallback = std::mem_fn(&PhysicsSystem::handleVelocityChange);
		_velocityChangeFilter.init(this, velocityChangeCallback);
		single<EventProcessor>().addFilter(&_velocityChangeFilter);


		std::function<void(PhysicsSystem*, AccelerationChangeEvent&)> accelerationChangeCallback = std::mem_fn(&PhysicsSystem::handleAccelerationChange);
		_accelerationChangeFilter.init(this, accelerationChangeCallback);
		single<EventProcessor>().addFilter(&_accelerationChangeFilter);

		std::function<void(PhysicsSystem*, ObjectCollisionQuery&)> objectCollisionCallback = std::mem_fn(&PhysicsSystem::answerObjectCollisionQuery);
		_objectCollisionFilter.init(this, objectCollisionCallback);
		single<EventProcessor>().addFilter(&_objectCollisionFilter);

		std::function<void(PhysicsSystem*, PositionChangeEvent&)> positionChangeCallback = std::mem_fn(&PhysicsSystem::handlePositionChange);
		_positionChangeFilter.init(this, positionChangeCallback);
		single<EventProcessor>().addFilter(&_positionChangeFilter);


		std::function<void(PhysicsSystem*, EntityPositionQuery&)> positionQueryCallback = std::mem_fn(&PhysicsSystem::answerEntityPositionQuery);
		_positionQueryFilter.init(this, positionQueryCallback);
		single<EventProcessor>().addFilter(&_positionQueryFilter);

		std::function<void(PhysicsSystem*, EntitiesInRectQuery&)> entitiesInRectQueryCallback = std::mem_fn(&PhysicsSystem::answerEntitiesInRectQuery);
		_entitiesInRectQueryFilter.init(this, entitiesInRectQueryCallback);
		single<EventProcessor>().addFilter(&_entitiesInRectQueryFilter);


		return System::initializeImpl();
	}

	InitStatus PhysicsSystem::resetImpl() {
		single<EventProcessor>().removeFilter(&_velocityChangeFilter);
		single<EventProcessor>().removeFilter(&_accelerationChangeFilter);
		single<EventProcessor>().removeFilter(&_objectCollisionFilter);
		single<EventProcessor>().removeFilter(&_positionChangeFilter);
		single<EventProcessor>().removeFilter(&_positionQueryFilter);
		single<EventProcessor>().removeFilter(&_entitiesInRectQueryFilter);
		_fixedPhysicsFacets.clear();
		_fixedPhysicsFacets.shrink_to_fit();

		return System::resetImpl();

	}

	void PhysicsSystem::destroyFacets(Entity& entity) {

		for (auto it = std::begin(_fixedPhysicsFacets); it != std::end(_fixedPhysicsFacets); ++it) {

			if (it->of() == entity) {
				it = _fixedPhysicsFacets.erase(it);
				return;
			}
		}
		for (auto it = std::begin(_physicsFacets); it != std::end(_physicsFacets); ++it) {
			if (it->of() == entity) {
				it = _physicsFacets.erase(it);
				return;
			}			
		}
	}


	void PhysicsSystem::updateImpl(RuntimeContext& context) {
		
		auto attemptingToMove = std::vector<PhysicsFacet*>{};


		//first pass eval proposed moves for anything that is not fixed
		for (unsigned i = 0; i < _physicsFacets.size(); i++)
		{

			auto& pf = _physicsFacets[i];
			if (pf.isPaused()) continue;
			pf.velocity += (pf.acceleration + _gravity) * (context.dt / 1000.0f);

			Vec2 thisMove = pf.velocity * (context.dt / 1000.0f) + pf.microMove;

			int wholex = (int)thisMove.x;
			int wholey = (int)thisMove.y;
			pf.microMove.x = thisMove.x - wholex;
			pf.microMove.y = thisMove.y - wholey;

			if (abs(wholex) > 0 || abs(wholey) > 0) {
				attemptingToMove.push_back(&pf);
				pf.proposedMove.x = pf.p.x + wholex;
				pf.proposedMove.y = pf.p.y + wholey;
			}

			for (unsigned i = 0; i < pf.collisionState.contactStates.size(); ++i) {
				auto& contact = pf.collisionState.contactStates[i];
				if (contact.state == -10) {
					pf.collisionState.contactStates.erase(i);
					--i;
				}
				else {
					--contact.state;
				}

			}

		}


		auto collisions = std::vector<Collision>{};

		//second pass check collisions
		for (auto pf : attemptingToMove) {

			auto collision = Collision{};

			collision.a = pf;
			getPositionValues(*pf, collision.ax, collision.ay);

			//check other dynamic facets
			for (auto& testFacet : _physicsFacets) {

				if (*pf == testFacet) continue;

				auto contact = Contact{};

				getPositionValues(testFacet, contact.bx, contact.by);
				contact.b = &testFacet;

				checkSingleContact(collision, contact);

			}

			//check fixed facets
			for (auto& testFacet : _fixedPhysicsFacets) {

				auto contact = Contact{};

				getPositionValues(testFacet, contact.bx, contact.by);
				contact.b = &testFacet;

				checkSingleContact(collision, contact);
			}

			if (!collision.contacts.empty()) solveCollision(collision);
			
			//check for an actual move

			if (pf->proposedMove != pf->p) {

				pf->p = pf->proposedMove;
				auto ev = PositionChangeEvent{};

				ev.setOriginator(this);
				ev.position.setPixel(pf->p);
				ev.entity = pf->of();				
				single<EventProcessor>().process(ev);

			}


		}
		
		for (auto& pf : _physicsFacets) {			
			//publish if there are updates to contact state
			if (!pf.collisionState.contactStates.empty()) {
				if (pf.collisionState.contactStates.size() > 1) {
					int x = 0;
				}
				single<EventProcessor>().process(pf.collisionState);
			}
		}

	}
	

	void PhysicsSystem::checkSingleContact(Collision& col, Contact& c) {

		c.collided = ((inRangeSorted(min(col.ax[0], col.ax[1]), max(col.ax[2], col.ax[3]), min(c.bx[0], c.bx[1]), max(c.bx[2], c.bx[3])) & inRangeSorted(min(col.ay[0], col.ay[1]), max(col.ay[2], col.ay[3]), min(c.by[0], c.by[1]), max(c.by[2], c.by[3]))) == 1);


		if (!c.collided) return;

		int velax = col.ax[1] - col.ax[0];
		int velay = col.ay[1] - col.ay[0];



		//find the adjustment for each axis
		//if the velocity is nonzero
		if (velax > 0) {
			c.adjA[0] = col.ax[3] - c.bx[1];
			c.subT[0] = (velax - c.adjA[0]) / velax;
			c.adjA[0]++;
		}
		else if (velax < 0) {
			c.adjA[0] = col.ax[1] - c.bx[3];
			c.subT[0] = (velax + c.adjA[0]) / velax;
			c.adjA[0]--;
		}
		else {
			c.subT[0] = 2;		//setting this to 2 
			//since this is normally a fraction <= 1
			//anything will compare less than this
		}
		if (c.subT[0] > 2) {

			int x = 0;
		}

		if (velay > 0) {
			c.adjA[1] = col.ay[3] - c.by[1];
			c.subT[1] = (velay - c.adjA[1]) / velay;
			c.adjA[1]++;
		}
		else if (velay < 0) {
			c.adjA[1] = col.ay[1] - c.by[3];
			c.subT[1] = (velay + c.adjA[1]) / velay;
			c.adjA[1]--;
		}
		else {
			c.subT[1] = 2; //this is a percent, so anything will turn out less than this
		}

		col.contacts.push_back(c);

	}


	void PhysicsSystem::solveCollision(Collision& col) {

				
		int velax = col.ax[1] - col.ax[0];

		int velay = col.ay[1] - col.ay[0];




		auto lvalueof1 = 1.0f;
		auto lvalueof0 = 0.0f;

		for (unsigned i = 0; i < col.contacts.size(); i++) {

			auto& ci = col.contacts[i];
			auto contactFound = false;
			for (unsigned contactStateIndex = 0; contactStateIndex < col.a->collisionState.contactStates.size(); ++contactStateIndex) {

				auto& contactState = col.a->collisionState.contactStates[contactStateIndex];
				if (col.a->collisionState.contactStates.size() > 1) {
					int x = 0;
				}
				if (contactState.entity == ci.b->of()) {
					contactFound = true;
					contactState.state = 0;
					contactState.contactNormal.setVec2(Vec2{ ci.adjA[0] * -1.0f, ci.adjA[1] * -1.0f });
					break;
				}
			}
			if (!contactFound) {
				auto& nested = col.a->collisionState.contactStates.emplace_back();
				nested.entity = ci.b->of();
				nested.blocking = ci.b->blocking;				
				nested.contactNormal.setVec2(Vec2{ ci.adjA[0] * -1.0f, ci.adjA[1] * -1.0f });
				nested.state = 1;
				//TODO this is not working
			}
			

			if (!ci.b->blocking) {
				int x = 0;
				continue;
			}

			for (unsigned j = i + 1; j < col.contacts.size(); j++) {

				auto& cj = col.contacts[j];

				if (!cj.b->blocking) continue;


				//and if moving down and both share a top section
				if (velay < 0 && ci.by[3] == cj.by[3]) {
					ci.adjA[0] = 0;
					cj.adjA[0] = 0;
					ci.subT[0] = 2;
					cj.subT[0] = 2;
				}
				else if (velay > 0 && ci.by[1] == cj.by[1]) {
					ci.adjA[0] = 0;
					cj.adjA[0] = 0;
					ci.subT[0] = 2;
					cj.subT[0] = 2;

				}



				//and if moving left and both share a right section
				if (velax < 0 && ci.bx[3] == cj.bx[3]) {
					ci.adjA[1] = 0;
					cj.adjA[1] = 0;
					ci.subT[1] = 2;
					cj.subT[1] = 2;
				}
				else if (velax > 0 && ci.bx[1] == cj.bx[1]) {
					ci.adjA[1] = 0;
					cj.adjA[1] = 0;
					ci.subT[1] = 2;
					cj.subT[1] = 2;
				}


			}
		}


		//now find the greatest adjustments
		for (unsigned i = 0; i < col.contacts.size(); i++) {

			auto& c = col.contacts[i];

			if (!c.b->blocking) continue;

			if (inRange(lvalueof0, col.subT[0], c.subT[0])) {
				col.subT[0] = c.subT[0];
				col.finalAdjA[0] = c.adjA[0];

			}
			if (inRange(lvalueof0, col.subT[1], c.subT[1])) {
				col.subT[1] = c.subT[1];
				col.finalAdjA[1] = c.adjA[1];
			}


		}


		if (inRange(lvalueof0, lvalueof1, col.subT[0]) && col.finalAdjA[0] != 0) {
			col.a->proposedMove.x -= (col.finalAdjA[0]);
			col.a->velocity.x = 0;
		}
		if (inRange(lvalueof0, lvalueof1, col.subT[1]) && col.finalAdjA[1] != 0) {
			col.a->proposedMove.y -= (col.finalAdjA[1]);
			col.a->velocity.y = 0;
		}	

		col.a->collisionState.newPosition.setPixel(col.a->proposedMove);
		col.a->collisionState.newVelocity.setVec2(col.a->velocity);

	}

	void PhysicsSystem::getPositionValues(PhysicsFacet& pf, int(&x)[4], int(&y)[4]) {
		int mw = pf.dim.w;
		int mh = pf.dim.h;

		x[0] = pf.p.x;
		x[1] = pf.proposedMove.x;
		x[2] = x[0] + mw;
		x[3] = x[1] + mw;

		y[0] = pf.p.y;
		y[1] = pf.proposedMove.y;
		y[2] = y[0] + mh;
		y[3] = y[1] + mh;
	}


	void PhysicsSystem::setPosition(Entity& e, Pixel p) {

		for (auto& facet : _physicsFacets) {

			if (facet.of() == e) {

				facet.p = p;

				auto pce = PositionChangeEvent{};
				pce.entity = e;				
				pce.position.setPixel(facet.p);			

				single<EventProcessor>().process(pce);

			}
		}

	}

	void PhysicsSystem::answerEntitiesInRectQuery(EntitiesInRectQuery& query) {

		int x1 = query.rect[0];
		int y1 = query.rect[1];
		int x2 = query.rect[2];
		int y2 = query.rect[3];

		int a1, b1, a2, b2;
		bool collided;
		for (PhysicsFacet& facet : _physicsFacets) {
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

		for (PhysicsFacet& facet : _fixedPhysicsFacets) {
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

	void PhysicsSystem::answerObjectCollisionQuery(ObjectCollisionQuery& query) {

		PhysicsFacet& concerningFacet = _physicsFacets[0];

		bool found = false;

		for (auto& facet : _physicsFacets) {
			if (facet.isPaused()) continue;
			if (facet.of() == query.concerning){
				concerningFacet = facet; 
				found = true;
				break;
			}
		}
		
		if (!found) return;

		int ax1 = concerningFacet.p.x;
		int ax2 = ax1 + concerningFacet.dim.w;
		int ay1 = concerningFacet.p.y;
		int ay2 = ay1 + concerningFacet.dim.h;


		for (auto& facet : _physicsFacets) {
				

			if (facet != concerningFacet) {

				int bx1 = facet.p.x;
				int by1 = facet.p.y;
				int bx2 = bx1 + facet.dim.w;
				int by2 = by1 + facet.dim.h;

				if (inRangeSorted(ax1, ax2, bx1, bx2) == 1 && inRangeSorted(ay1, ay2, by1, by2) == 1) {
					query.contacts.push_back(facet.of());
				}
			}
		}

		for (auto& facet : _fixedPhysicsFacets) {


			if (facet != concerningFacet) {

				int bx1 = facet.p.x;
				int by1 = facet.p.y;
				int bx2 = bx1 + facet.dim.w;
				int by2 = by1 + facet.dim.h;

				if (inRangeSorted(ax1, ax2, bx1, bx2) == 1 && inRangeSorted(ay1, ay2, by1, by2) == 1) {
					query.contacts.push_back(facet.of());
				}
				
			}
		}

	}

	EventProcessor::EventRegistration<EntityPositionQuery> entityPositionQueryReg{};
	EventProcessor::EventRegistration<VelocityChangeEvent> velChangeEventReg{};
	EventProcessor::EventRegistration<PositionChangeEvent> positionChangeEventReg{};
	EventProcessor::EventRegistration<EntitiesInRectQuery> entitiesInRectQueryReg{};

	int PhysicsSystem::createFacet_bind(LuaState& lua) {

		auto systemName = lua.pullStack<std::string>(1);
		auto entity = lua.pullStack<int>(2);

		auto system = single<Core>().getSystemByName<PhysicsSystem>(systemName);

		if (system != nullptr) {
			if (entity == 437) {
				int x = 0;
			}
			auto& facet = system->createPhysicsFacet(entity, lua["fixed"]);



			LuaPixel position = lua["position"];
			facet.p = position.getPixel();			
			facet.proposedMove = facet.p;


			LuaDimension dimensions = lua["dimensions"];
			facet.dim.w = dimensions.width();
			facet.dim.h = dimensions.height();			

			LuaVec2 velocity = lua["velocity"];
			facet.velocity = velocity.getVec2();

			LuaVec2 acceleration = lua["acceleration"];
			facet.acceleration = acceleration.getVec2();			

			facet.mass = lua["mass"];
			facet.blocking = lua["blocking"];

			lua.pushStack(facet.id());

		}
		else {
			lua.pushStack(-1L);
		}



		return 1;
	}

} //end namespace core
