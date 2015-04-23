#ifndef CORE_LUA_TABLE_HPP
#define CORE_LUA_TABLE_HPP

#include <string>
#include <map>
#include <algorithm>
#include <typeindex>
#include <vector>
#include "lua.hpp"
#include "Geometry.hpp"
#include "LuaFunction.hpp"


namespace core {

	class LuaState;


	class LuaTable {
		friend class LuaState;
	public:

		LuaTable();

		LuaTable(const LuaTable&& other);

		LuaTable& operator=(LuaTable&& other);

		LuaTable& operator=(LuaState& state);

		LuaTable(LuaTable&& other) {
			int x = 0;
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

		void clear();

		void erase(int index);

		void erase(std::string key);
		

		LuaTable& operator[](int key);

		LuaTable& operator[](const char* key);

		operator unsigned short() const;

		operator short() const;

		operator long() const;

		operator int() const;

		operator unsigned int() const;

		operator float() const;

		operator bool() const;

		operator std::string() const;

		operator LuaFunction() const;

		virtual void toLua(LuaState& lua) const;

		void pushAnyValue(LuaState& lua, std::type_index type, void* value) const;


		void pullAnyValue(LuaState& lua, std::type_index type, void *value);

		virtual void fromLua(LuaState& lua);


		int size() const;

		int stringKeySize() const;
		int intKeySize() const;

		protected:

		LuaTable& indexOpOverload(unsigned valuesKey);

		std::map<int, unsigned> intKeys;
		std::map<std::string, unsigned> stringKeys;


		std::vector<std::type_index> types;
		std::vector<void*> values;

		void* value;

	};




} //end namespace core



#endif