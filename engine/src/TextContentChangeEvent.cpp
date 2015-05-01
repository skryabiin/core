#include "TextContentChangeEvent.hpp"
#include "EventProcessor.hpp"

namespace core  {


	TextContentChangeEvent::TextContentChangeEvent() : facetId{ -1 } {
		
		lua_reg("entityId", &entity);
		lua_reg("textContent", &textContent);
		lua_reg("font", &font);
		lua_reg("facetId", &facetId);

	}

	std::string TextContentChangeEvent::getEventTypeNameImpl() {
		return "TextContentChangeEvent";
	}


	bool TextContentChangeEvent::createFromLua(LuaState& lua) {

		auto tcc = TextContentChangeEvent{};
		tcc.fromLua(lua);
		single<EventProcessor>().process(tcc);
		return false;

	}



} //end namespace core