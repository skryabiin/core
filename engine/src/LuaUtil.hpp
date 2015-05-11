#ifndef CORE_LUA_UTIL_H
#define CORE_LUA_UTIL_H

#include <string>
#include "lua.h"
#include "Geometry.hpp"


namespace core {

	template <typename T>
	inline void pushLuaVec(lua_State* L, T x, T y) {
		lua_createtable(L, 2, 0);
		setLuaTable(L, 1, x);
		setLuaTable(L, 2, y);
	}

	template <typename T, typename U>
	inline void setLuaTable(lua_State* L, T key, U value) {
		luaPushCast<T>(L, key);
		luaPushCast<U>(L, value);
		lua_settable(L, -3);
	}

	template <typename T, typename U>
	inline U luaTableValue(lua_State* L, T key) {
		luaPushCast<T>(L, key);
		lua_gettable(L, -2);
		U out = luaCast<U>(L, -1);
		lua_pop(L, 1);
		return out;
	}

	template <typename T>
	inline void luaPushCast(lua_State* L, T t) {

	}

	template<>
	inline void luaPushCast(lua_State* L, int t) {
		lua_pushinteger(L, t);
	}

	template<>
	inline void luaPushCast(lua_State* L, float t) {
		lua_pushnumber(L, t);
	}

	template<>
	inline void luaPushCast(lua_State* L, double t) {
		lua_pushnumber(L, t);
	}

	template<>
	inline void luaPushCast(lua_State* L, std::string t) {
		lua_pushstring(L, t.c_str());
	}

	template<>
	inline void luaPushCast(lua_State* L, bool t) {
		lua_pushboolean(L, t);
	}

	template<>
	inline void luaPushCast(lua_State* L, void* t) {
		lua_pushlightuserdata(L, t);
	}

	template<typename T>
	inline T luaCast(lua_State* L, int stackIndex) {
		return T{};
	}

	template<>
	inline std::string luaCast(lua_State* L, int stackIndex) {
		if (lua_isstring(L, stackIndex)) {
			return lua_tostring(L, stackIndex);
		}
		else {
			return "Cannot cast lua value to string.";
		}
	}

	template<>
	inline int luaCast(lua_State* L, int stackIndex) {
		if (lua_isnumber(L, stackIndex)) {
			return lua_tointeger(L, stackIndex);
		}
		else {
			return 0;
		}
	}

	template<>
	inline double luaCast(lua_State* L, int stackIndex) {
		if (lua_isnumber(L, stackIndex)) {
			return lua_tonumber(L, stackIndex);
		}
		else {
			return 0.0;
		}
	}

	template<>
	inline float luaCast(lua_State* L, int stackIndex) {
		if (lua_isnumber(L, stackIndex)) {
			return float(lua_tonumber(L, stackIndex));
		}
		else {
			return 0.0f;
		}
	}


	template<>
	inline bool luaCast(lua_State* L, int stackIndex) {
		if (lua_isboolean(L, stackIndex)) {
			return lua_toboolean(L, stackIndex) == 1;
		}
		else {
			return false;
		}
	}



	template <typename Key_type, typename Value_type>
	struct lua_var {

		Key_type key;
		Value_type value;

		lua_var() {}

		lua_var(Key_type kkey) {
			key = kkey;
		}

		lua_var(Key_type kkey, Value_type vvalue) {
			key = kkey;
			value = vvalue;
		}

		lua_var& operator=(Value_type vvalue) {
			this->value = vvalue;
			return *this;
		}

		Value_type operator()() {
			return value;
		}

		void pushLuaStack(lua_State* L) {
			luaPushCast<Value_type>(L, value);
		}

		void pullLuaStack(lua_State* L) {
			value = luaCast<Value_type>(L);
		}

		void setLuaStack(lua_State* L) {
			setLuaTable<Key_type, Value_type>(L, key, value);
		}

		void getLuaStack(lua_State* L) {
			value = luaTableValue<Key_type, Value_type>(L, key);
		}

	};

	template<typename Key_type>
	struct lua_int : public lua_var<Key_type, int> {

		lua_int() {}

		lua_int& operator=(int vvalue) {
			this->value = vvalue;
			return *this;
		}

		lua_int(Key_type kkey) : lua_var{ kkey } {}

		lua_int(Key_type kkey, int vvalue) : lua_var{ kkey, vvalue } {}


	};

	template<typename Key_type>
	struct lua_string : public lua_var<Key_type, std::string> {


		lua_string() {}

		lua_string& operator=(std::string vvalue) {
			this->value = vvalue;
			return *this;
		}

		lua_string(Key_type kkey) : lua_var{ kkey } {}

		lua_string(Key_type kkey, std::string vvalue) : lua_var{ kkey, vvalue } {}

	};


	template<typename Key_type>
	struct lua_double : public lua_var<Key_type, double> {
		lua_double() {}
		
		lua_double& operator=(double vvalue) {
			this->value = vvalue;
			return *this;
		}


		lua_double(Key_type kkey) : lua_var{ kkey } {}

		lua_double(Key_type kkey, double vvalue) : lua_var{ kkey, vvalue } {}

	};

	template<typename Key_type>
	struct lua_float : public lua_var<Key_type, float> {
		lua_float() {}

		lua_float& operator=(float vvalue) {
			this->value = vvalue;
			return *this;
		}

		lua_float(Key_type kkey) : lua_var{ kkey } {}

		lua_float(Key_type kkey, float vvalue) : lua_var{ kkey, vvalue } {}
	};

	template<typename Key_type>
	struct lua_bool : public lua_var<Key_type, bool> {

		lua_bool() {}

		lua_bool& operator=(bool vvalue) {
			this->value = vvalue;
			return *this;
		}

		lua_bool(Key_type kkey) : lua_var{ kkey } {}

		lua_bool(Key_type kkey, bool vvalue) : lua_var{ kkey, vvalue } {}
	};




	template <typename Value_type, typename Key_type, int arrayValues = 0, int fields = 0>
	struct lua_table {

		lua_table() {}

		lua_table(Key_type kkey) {
			key = kkey;
		}

		Key_type key;

		void setLuaStack(lua_State* L) {
			luaPushCast<Key_type>(L, key);
			pushLuaStack(L);
			lua_settable(L, -3);
		}

		void pushLuaStack(lua_State* L) {
			lua_createtable(L, arrayValues, fields);
			static_cast<Value_type*>(this)->setLuaStackImpl(L);
		}

		void getLuaStack(lua_State* L) {
			luaPushCast<Key_type>(L, key);
			lua_gettable(L, -2);
			pullLuaStack(L);
		}

		void pullLuaStack(lua_State* L) {
			static_cast<Value_type*>(this)->getLuaStackImpl(L);
			lua_pop(L, 1);
		}


	};





} //end namespace core





#endif
