#ifndef CORE_PHYSICS_FACET_HPP
#define CORE_PHYSICS_FACET_HPP


#include "PositionFacet.hpp"
#include "Geometry.hpp"
#include "CollisionState.hpp"

namespace core {



	struct PhysicsFacet : public PositionFacet {

		PhysicsFacet() : PositionFacet{}, velocity{ 0.0f, 0.0f }, acceleration{ 0.0f, 0.0f }, mass{ 1.0f }, proposedMove{ 0, 0, 0 }, microMove{ 0.0f, 0.0f }, blocking{ false }, fixed{ true }  {
			_typeInfo = &typeid(*this);
		};

		Vec2 velocity;
		Vec2 acceleration;
		float mass;
		bool blocking;
		bool fixed;
		Pixel proposedMove;
		Vec2 microMove;

		CollisionState collisionState;
		

	};




} //end namespace core

#endif