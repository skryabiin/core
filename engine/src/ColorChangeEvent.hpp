#ifndef CORE_COLOR_CHANGE_EVENT_HPP 
#define CORE_COLOR_CHANGE_EVENT_HPP

#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {



	struct ColorChangeEvent : public Event<ColorChangeEvent> {

		ColorChangeEvent();

		Entity entity;
		LuaColor color;		
		int facetId;
		bool doModulateColor;
		int blendMode;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

	};



} //end namespace core

#endif