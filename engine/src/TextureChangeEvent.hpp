#ifndef CORE_TEXTURE_CHANGE_EVENT_HPP
#define CORE_TEXTURE_CHANGE_EVENT_HPP

#include "Event.hpp"
#include "LuaState.hpp"
#include "Core.hpp"

namespace core {


	struct TextureChangeEvent : public Event<TextureChangeEvent> {



		TextureChangeEvent();

		Entity entity;
		std::string textureName;
		LuaRect sourceTextureRect;
		int facetId;

		static bool createFromLua(LuaState& lua);

		static std::string getEventTypeNameImpl();

	};


} //end namespace core

#endif