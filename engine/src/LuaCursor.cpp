#include "LuaState.hpp"
#include "LuaCursor.hpp"
#include "lua.hpp"
#include "LuaTable.hpp"


namespace core {

	LuaCursor::LuaCursor() {}

	LuaCursor::LuaCursor(LuaCursor& other) {
		_L = other._L;
	}

	LuaCursor& LuaCursor::operator=(LuaCursor& other) {
		_L = other._L;
		return *this;
	}

	LuaCursor& LuaCursor::operator[](int index) {

		lua_pushinteger(_L, index);
		lua_gettable(_L, -2);
		return *this;
	}

	LuaCursor& LuaCursor::operator[](const char* key) {

		lua_pushstring(_L, key);
		lua_gettable(_L, -2);
		return *this;
	}


	void LuaCursor::getTable(std::string name) {
		lua_pushstring(_L, name.c_str());
		lua_gettable(_L, -2);

	}

	void LuaCursor::getTable(int index) {
		lua_pushinteger(_L, index);
		lua_gettable(_L, -2);
	}

	void LuaCursor::foreachKey(std::function<void(std::string, LuaCursor&)> fun) {
		lua_pushnil(_L);
		while (lua_next(_L, -2) != 0) {
			fun(lua_tostring(_L, -2), *this);
			lua_pop(_L, 1);
		}
		lua_pop(_L, 1);
	}

	void LuaCursor::foreachIndex(std::function<void(int, LuaCursor&)> fun) {

		lua_pushnil(_L);
		while (lua_next(_L, -2) != 0) {
			fun(lua_tointeger(_L, -2), *this);
			lua_pop(_L, 1);
		}
		lua_pop(_L, 1);
	}

	LuaCursor::operator bool() {
		return lua_toboolean(_L, -1) == 1;
	}

	LuaCursor::operator int() {
		return lua_tointeger(_L, -1);
	}

	LuaCursor::operator float() {
		return float(lua_tonumber(_L, -1)); 
	}

	LuaCursor::operator std::string() {
		return lua_tostring(_L, -1);		
	}


} //end namespace core