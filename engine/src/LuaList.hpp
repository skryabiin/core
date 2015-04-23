#ifndef CORE_LUA_LIST_HPP
#define CORE_LUA_LIST_HPP

#include <list>
#include "LuaTable.hpp"


namespace core {

	class LuaState;

	template <typename T>
	class LuaList : public LuaTable {


	public:
		
		void erase(int index) {		
			auto it = std::begin(_values);
			for (int i = 0; i < index; ++i) {
				++it;
			}
			_values.erase(it);
			LuaTable::erase(index + 1);
		}
		
		void clear() {
			_values.clear();
			LuaTable::clear();
		}

		
		T& emplace_back() {
			int x = 0;
			_values.emplace_back();
			lua_reg(_values.size(), &_values.back());
			return _values.back();
		}

		void push_back(T& listElement) {

			_values.push_back(std::move(listElement));
			lua_reg(_values.size(), &_values.back());
			
		}

		virtual void toLua(LuaState& lua) const;

		virtual void fromLua(LuaState& lua);
		


		std::list<T>& values() {
			return _values;
		}

		template <typename T>
		void setValueFromState(LuaState& lua, T& t) {
			static_cast<LuaTable>(t) = lua;
		}

		template <>
		void setValueFromState<bool>(LuaState& lua, bool& t) {
			t = lua;
		}

		template <>
		void setValueFromState<int>(LuaState& lua, int& t) {
			t = lua;
		}

		template <>
		void setValueFromState<float>(LuaState& lua, float& t) {
			t = lua;
		}

		template <>
		void setValueFromState<std::string>(LuaState& lua, std::string& t) {
			t = lua;
		}

		T& operator[](int index) {	
			auto it = _values.begin();
			for (int i = 0; i < index; ++i) {
				++it;
			}
			return *(it);
		}
		
		unsigned size() {
			return std::distance(_values.begin(), _values.end());
		}
		
		bool empty() {
			return _values.empty();
		}

	protected:

		std::list<T> _values;

	};

	template <typename T>
	void LuaList<T>::toLua(LuaState& lua) const {

		lua.createTable(_values.size(), 0);

		int index = 0;
		for (auto it = std::begin(_values); it != std::end(_values); ++it) {
			++index;
			lua.pushStack(index);
			lua.pushStack(*it);
			lua.setTable();			
		}
		int x = 0;
	}

	template <typename T>
	void LuaList<T>::fromLua(LuaState& lua) {

		_values.clear();
		lua.foreachIndex([&](const int key, LuaState& lua)->void {
			auto element = T{};
			setValueFromState(lua, element);
			_values.push_back(element);
		});
	}





} //end namespace core

#endif