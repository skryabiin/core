#ifndef CORE_MENU_HPP
#define CORE_MENU_HPP


#include "LuaTable.hpp"
#include "LuaList.hpp"
#include "MenuOption.hpp"

namespace core {

	struct Menu : public LuaTable {
		
		Menu() {

			lua_reg("options", &options);
			lua_reg("position", &position);
			lua_reg("menuKeyScancode", &menuKeyScancode);
			lua_reg("backgroundTexture", &backgroundTexture);
			lua_reg("backgroundTextureAlpha", &backgroundTextureAlpha);
		}

		LuaList<MenuOption> options;
		LuaPixel position;
		LuaDimension dimensions;
		int menuKeyScancode;
		std::string backgroundTexture;
		int backgroundTextureAlpha;
		Menu* parent;

	};



} //end namespace core
#endif