#ifndef CORE_COLOR_MODULATION_EVENT_HPP
#define CORE_COLOR_MODULATION_EVENT_HPP

#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {


	struct ColorModulationEvent : public Event<ColorModulationEvent> {

		ColorModulationEvent();

		Entity entity;
		LuaVector<float, 16> matrix;
		int facetId;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);


	};


} //end namespace core

#endif