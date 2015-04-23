#include <vector>
#include <map>
#include <typeindex>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <tchar.h>

#include "lua.hpp"


inline std::string dumpLuaStack(lua_State *L)
{
	int i;
	int top = lua_gettop(L);

	std::ostringstream ss;

	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

		case LUA_TSTRING:  /* strings */
			ss << lua_tostring(L, i);
			break;

		case LUA_TBOOLEAN:  /* booleans */
			ss << (lua_toboolean(L, i) ? "true" : "false");
			break;

		case LUA_TNUMBER:  /* numbers */
			ss << lua_tonumber(L, i);
			break;

		default:  /* other values */
			ss << lua_typename(L, t);
			break;

		}
		ss << "  ";  /* put a separator */
	}

	return ss.str();
}


struct lua_struct  {

	lua_struct() {

	}


	template <typename var_type>
	void lua_reg(std::string key, var_type* var) {
		types.push_back(typeid(var_type));
		values.push_back(var);
		auto newIndex = values.size() - 1;
		stringKeys[key] = newIndex;
	}

	template<typename table_type>
	void lua_reg_table(std::string key, table_type* var) {
		types.push_back(typeid(lua_struct));
		values.push_back(var);
		auto newIndex = values.size() - 1;
		stringKeys[key] = newIndex;
	}

	template <typename var_type>
	void lua_reg(int key, var_type* var) {
		types.push_back(typeid(var_type));
		values.push_back(static_cast<void*>(var));
		auto newIndex = values.size() - 1;
		intKeys[key] = newIndex;
	}

	template<typename table_type>
	void lua_reg_table(int key, table_type* var) {
		types.push_back(typeid(lua_struct));
		values.push_back(var);
		auto newIndex = values.size() - 1;
		intKeys[key] = newIndex;
	}

	/*
	lua_struct operator[](int key) {

		auto valuesKey = intKeys[key];
		if (types[valuesKey] == typeid(lua_struct)) {
			return *(static_cast<lua_struct*>(values[valuesKey]));
		}
		auto out = lua_struct{};
		out.value = values[valuesKey];
		return out;
	}*/

	operator int() const {
		return *(static_cast<int*>(value));
	}

	operator float() const {
		return *(static_cast<float*>(value));
	}

	void toLua(lua_State* L) {
		lua_createtable(L, intKeys.size(), stringKeys.size());

		std::for_each(std::begin(intKeys), std::end(intKeys),
			[&](std::pair<const int, unsigned> intKey)->void {

			lua_pushinteger(L, intKey.first);

			pushAnyValue(L, types[intKey.second], values[intKey.second]);

			std::cout << dumpLuaStack(L) << std::endl;
			lua_settable(L, -3);

		});

		std::for_each(std::begin(stringKeys), std::end(stringKeys),
			[&](std::pair<const std::string, unsigned> stringKey)->void {

			lua_pushstring(L, stringKey.first.c_str());

			pushAnyValue(L, types[stringKey.second], values[stringKey.second]);

			lua_settable(L, -3);
	
		});

	}

	void pushAnyValue(lua_State* L, std::type_index type, void* value) {

		std::type_index thisType = typeid(int);

		if (type == typeid(lua_struct)) {
			static_cast<lua_struct*>(value)->toLua(L);
		}
		else if (type == typeid(int)) {
			lua_pushinteger(L, *static_cast<int*>(value));
		}
		else if (type == typeid(std::string)) {
			lua_pushstring(L, static_cast<std::string*>(value)->c_str());
		}
		else if (type == typeid(bool)) {
			lua_pushboolean(L, *static_cast<bool*>(value));
		}
		else if (type == typeid(float)) {
			lua_pushnumber(L, *static_cast<float*>(value));
		}
		else if (type == typeid(double)) {
			lua_pushnumber(L, *static_cast<double*>(value));
		}
		else {
			lua_pushnil(L);
		}

	}

	void pullAnyValue(lua_State* L, std::type_index type, void *value) {

		if (type == typeid(lua_struct)) {
			static_cast<lua_struct*>(value)->fromLua(L);
		}
		else if (type == typeid(int)) {
			*static_cast<int*>(value) = lua_tointeger(L, -1);
		}
		else if (type == typeid(std::string)) {
			*static_cast<std::string*>(value) = lua_tostring(L, -1);
		}
		else if (type == typeid(float)) {
			*static_cast<float*>(value) = float(lua_tonumber(L, -1));
		}
		else if (type == typeid(double)) {
			*static_cast<double*>(value) = lua_tonumber(L, -1);
		}
		else if (type == typeid(bool)) {
			*static_cast<bool*>(value) = lua_toboolean(L, -1) == 1;
		}

	}

	void fromLua(lua_State* L) {

		std::for_each(std::begin(intKeys), std::end(intKeys),
			[&](std::pair<const int, unsigned> intKey)->void {

			lua_pushinteger(L, intKey.first);
			lua_gettable(L, -2);

			pullAnyValue(L, types[intKey.second], values[intKey.second]);

			lua_pop(L, 1);
		});

		std::for_each(std::begin(stringKeys), std::end(stringKeys),
			[&](std::pair<const std::string, unsigned> stringKey)->void {

			lua_pushstring(L, stringKey.first.c_str());
			lua_gettable(L, -2);

			pullAnyValue(L, types[stringKey.second], values[stringKey.second]);

			lua_pop(L, 1);

		});

	}

	std::map<int, unsigned> intKeys;
	std::map<std::string, unsigned> stringKeys;


	std::vector<std::type_index> types;
	std::vector<void*> values;

	void* value;

};


template <typename T, int dimensions = 2>
class LuaVector : public lua_struct {
public:

	LuaVector() : _values{ dimensions } {
		

		for (unsigned i = 1; i <= dimensions; ++i) {

			lua_reg(i, &_values[i-1]);
		}
	}

	T& operator[](unsigned index) {
		return _values[index - 1];
	}

private:
	std::vector<T> _values;
};


struct MyOtherLuaThing : public lua_struct {

	MyOtherLuaThing() :
	a{ 0 },
	b{ 0 }
	{
		lua_reg(1, &a);
		lua_reg(2, &b);
	}

	int a;
	int b;
};

struct MyLuaThing : public lua_struct {

	MyLuaThing() :
	x{ 0 },
	y{ 0 } {
		lua_reg(1, &x);
		lua_reg("c", &y);
		lua_reg_table("nested", &nested);
	}


	int x;
	int y;
	MyOtherLuaThing nested;

};

class LuaState {

public:

	LuaState() : _L{ nullptr } {

		_L = luaL_newstate();

		luaL_openlibs(_L);

	}


	LuaState& operator[](int index) {


	}

	LuaState& operator[](const char* index) {

	}


	void clearStack() {
		lua_pop(_L, lua_gettop(_L));
	}


	bool runString(std::string luaCode) {
		return luaL_dostring(_L, luaCode.c_str());
	}

	template<typename T>
	void pushStack(T& value) {

	}

	template<>
	void pushStack<int>(int& value) {
		lua_pushinteger(_L, value);
	}

	template<>
	void pushStack<std::string>(std::string& value) {
		lua_pushstring(_L, value.c_str());
	}

	template<>
	void pushStack<float>(float& value) {
		lua_pushnumber(_L, value);
	}

	template<>
	void pushStack<double>(double& value) {
		lua_pushnumber(_L, value);
	}

	template<>
	void pushStack<bool>(bool& value) {
		lua_pushboolean(_L, value);
	}

	template<>
	void pushStack<std::nullptr_t>(std::nullptr_t& np) {
		lua_pushnil(_L);
	}


	template<typename ...Args>
	void call(std::string functionName, Args... args) {

		//clear the stack for this
		lua_pop(_L, lua_gettop(_L));
		lua_getglobal(_L, functionName.c_str());
		auto numArgs = 0;
		callRec(numArgs, args...);

		lua_pcall(_L, numArgs, LUA_MULTRET, 0);


	}


	template<typename Arg1>
	void callRec(int& numArgs, Arg1 arg1) {
		numArgs++;
		pushStack(arg1);
	}

	template<typename Arg1, typename Arg2>
	void callRec(int& numArgs, Arg1 arg1, Arg2 arg2) {
		numArgs++;
		pushStack(arg1);
		callRec(numArgs, arg2);
	}

	template<typename Arg1, typename Arg2, typename ...Args>
	void callRec(int& numArgs, Arg1 arg1, Arg2 arg2, Args... args) {
		numArgs++;
		pushStack(arg1);
		callRec(numArgs, arg2, args...);

	}


	~LuaState() {
		lua_close(_L);

	}

	lua_State* _L;

private:




};




int _tmain(int argc, _TCHAR* argv[])
{



	auto testVector = LuaVector<int, 3>{};
	testVector[1] = 4;

	testVector[3] = 6;
	testVector[2] = 5;
	testVector[1] = 7;
	auto L = LuaState{};

	testVector.toLua(L._L);

	std::cout << dumpLuaStack(L._L) << std::endl;
	lua_setglobal(L._L, "vector");
	
	std::cout << "Running default script test.lua...\n" << std::endl;

	int result = luaL_dofile(L._L, "test.lua");

	if (result != 0) {
		std::cout << "Error running script: " << lua_tostring(L._L, -1) << std::endl;
		lua_pop(L._L, 1);

	} 
	else { 

		
		bool doQuit = false;
		bool collectReturn = true;

		while (!doQuit) {

			std::cout << ">";
			auto com = std::string{};

			std::getline(std::cin, com);

			if (!com.substr(0, 5).compare("`quit") || !(com.substr(0, 5).compare("`exit"))) {
				doQuit = true;
				continue;
			}
			if (!com.substr(0, 5).compare("`test")) {

				lua_getglobal(L._L, "vector");
				testVector.fromLua(L._L);				
			}
			if (!com.substr(0, 6).compare("`print")) {
				std::cout << testVector[1] << " " << testVector[2] << " " << testVector[3] << std::endl;
			}
			if (!com.substr(0, 6).compare("`stack")) {
				if (com.length() > 6) {

					auto param = com.substr(7);
					std::cout << "'" << param << "'" << std::endl;
					if (!param.compare("on")) {
						collectReturn = true;
						std::cout << "Stack on." << std::endl;
					}
					else if (!param.compare("off")) {
						collectReturn = false;
						std::cout << "Stack off." << std::endl;
					}
					else if (!param.compare("clear")) {
						L.clearStack();
						std::cout << "Stack cleared." << std::endl;
					}
				}
				else {
					std::cout << "Stack is " << ((collectReturn) ? "on" : "off") << ". Contents: " << std::endl << dumpLuaStack(L._L) << std::endl;
				}
				continue;
			}
			if (collectReturn) {
				com = std::string{ "return " }.append(com);
			}
			if (L.runString(com.c_str())) {
				//std::cout << "Error running line: " << lua_tostring(L, -1) << std::endl;
			}
		}


	}


	

	return 0;
}