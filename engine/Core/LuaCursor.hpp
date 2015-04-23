#ifndef CORE_LUA_CURSOR_HPP
#define CORE_LUA_CURSOR_HPP

#include <string>
#include <functional>
#include "lua.hpp"


namespace core {

	class LuaCursor {
		friend class LuaState;
	public:
		LuaCursor();
		
		LuaCursor(LuaCursor& other);
		
		bool next();


		void foreachKey(std::function<void(std::string, LuaCursor&)>);
		void foreachIndex(std::function<void(int, LuaCursor&)>);


		void getTable(std::string name);
		void getTable(int index);


		LuaCursor& operator=(LuaCursor& other);
		LuaCursor& operator[](int index);
		LuaCursor& operator[](const char* key);

		operator bool();
		operator int();
		operator float();
		operator std::string();		

	private:
		lua_State* _L;
	};



} //end namespace core

#endif