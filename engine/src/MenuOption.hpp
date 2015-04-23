#ifndef CORE_MENU_OPTION_HPP
#define CORE_MENU_OPTION_HPP

#include "LuaVector.hpp"
#include "RuntimeContext.hpp"
#include "LuaTable.hpp"
#include "Entity.hpp"

namespace core {

	struct MenuOptionDef : public LuaTable {

		LuaFunction onClick;
		Entity entity;
		LuaPixel relativePosition;
	};

	class MenuOption :  public nameable {
	public:

		MenuOption(MenuOptionDef definition);

		void click();


		~MenuOption();
	private:
		Pixel _relativePosition;		
		Entity _entity;
		LuaFunction _onClick;


	};


} //end namespace core
#endif