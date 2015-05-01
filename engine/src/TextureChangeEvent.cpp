#include "TextureChangeEvent.hpp"
#include "Console.hpp"
#include "EventProcessor.hpp"

namespace core {



	TextureChangeEvent::TextureChangeEvent() {
		lua_reg("entityId", &entity);
		lua_reg("textureName", &textureName);
		lua_reg("sourceTextureRect", &sourceTextureRect);
		lua_reg("facetId", &facetId);
	}

	std::string TextureChangeEvent::getEventTypeNameImpl() {

		return "TextureChangeEvent";
	}

	bool TextureChangeEvent::createFromLua(LuaState& lua) {

		auto tce = TextureChangeEvent{};

		tce.fromLua(lua);
		single<EventProcessor>().process(tce);

		return true;
	}



} //end namespace core