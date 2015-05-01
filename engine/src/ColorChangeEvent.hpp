#ifndef CORE_COLOR_CHANGE_EVENT_HPP 
#define CORE_COLOR_CHANGE_EVENT_HPP

#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {



	struct ColorChangeEvent : public Event<ColorChangeEvent> {

		ColorChangeEvent();

		Entity entity;	
		int facetId;
		LuaVector<float, 16> colorMatrix;
		LuaColor color;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

	};



} //end namespace core

#endif