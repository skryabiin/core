#ifndef CORE_OBJECT_COLLISION_QUERY_HPP
#define CORE_OBJECT_COLLISION_QUERY_HPP


#include "Event.hpp"
#include "Entity.hpp"

namespace core {



	struct ObjectCollisionQuery : public Event<ObjectCollisionQuery> {

		Entity concerning;

		std::vector<Entity> contacts;

		static std::string getEventTypeNameImpl() {

			return "ObjectCollisionQuery";
		}

	};




} //end namespace core




#endif