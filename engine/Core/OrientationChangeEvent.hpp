#ifndef CORE_ORIENTATION_CHANGE_EVENT_HPP
#define CORE_ORIENTATION_CHANGE_EVENT_HPP

#include "Core.hpp"
#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {


	struct OrientationChangeEvent : public Event<OrientationChangeEvent> {

		OrientationChangeEvent();

		Entity entity;
		LuaVec2 orientation;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

	};


} //end namespace core

#endif