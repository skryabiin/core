#ifndef CORE_DIMENSION_CHANGE_EVENT_HPP
#define CORE_DIMENSION_CHANGE_EVENT_HPP

#include "Core.hpp"
#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {


	struct DimensionChangeEvent : public Event<DimensionChangeEvent> {

		DimensionChangeEvent();

		Entity entity;
		LuaDimension dimensions;		

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

	};

} //end namespace core

#endif