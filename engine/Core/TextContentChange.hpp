#ifndef CORE_TEXT_CONTENT_CHANGE_HPP
#define CORE_TEXT_CONTENT_CHANGE_HPP

#include "Core.hpp"
#include "LuaUtil.hpp"
#include "Event.hpp"
#include "Entity.hpp"

namespace core {

	struct TextContentChange : public Event<TextContentChange> {

		TextContentChange();

		Entity entity;

		std::string textContent;

		LuaPixel offset;

		std::string font;
		
		LuaColor color;

		int facetId;

		static std::string getEventTypeNameImpl();

		static bool createFromLua(LuaState& lua);

	};



} //end namespace core

#endif