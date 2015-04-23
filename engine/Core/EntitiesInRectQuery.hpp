#ifndef CORE_ENTITIES_AT_RECT_QUERY
#define CORE_ENTITIES_AT_RECT_QUERY

#include "Event.hpp"
#include "LuaVector.hpp"
#include "LuaList.hpp"

namespace core {


	struct EntitiesInRectQuery : public Event<EntitiesInRectQuery> {


		EntitiesInRectQuery();


		LuaRect rect;
		LuaList<int> entities;

		static std::string getEventTypeNameImpl() {

			return "EntitiesInRectQuery";
		}

		static bool createFromLua(LuaState& lua);

	};


} //end namespace core

#endif