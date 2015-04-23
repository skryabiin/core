#include "TextContentChange.hpp"
#include "EventProcessor.hpp"

namespace core  {


	TextContentChange::TextContentChange() : facetId{ -1 } {
		
		lua_reg("entityId", &entity);
		lua_reg("textContent", &textContent);
		lua_reg("offset", &offset);
		lua_reg("font", &font);
		lua_reg("color", &color);
		lua_reg("facetId", &facetId);

	}

	std::string TextContentChange::getEventTypeNameImpl() {
		return "TextContentChange";
	}


	bool TextContentChange::createFromLua(LuaState& lua) {

		auto tcc = TextContentChange{};
		tcc.fromLua(lua);
		single<EventProcessor>().process(tcc);
		return false;

	}



} //end namespace core