#include "LuaMap.hpp"
#include "LuaState.hpp"

namespace core {



	LuaMap& LuaMap::operator=(std::map<std::string, std::string> map) {
			std::for_each(std::begin(map), std::end(map),
				[&](std::pair<std::string, std::string> element)->void {
				_values[element.first] = element.second;
				lua_reg(element.first, &_values[element.first]);
			});
			return *this;
		}

	void LuaMap::erase(std::string key) {
			LuaTable::erase(key);
			_values.erase(_values.find(key));
		}

		std::string& LuaMap::operator[](const char* key) {
			return _values[key];
		}

	
		void LuaMap::toLua(LuaState& lua) const {

			lua.createTable(0, _values.size());

			std::for_each(std::begin(_values), std::end(_values), [&](std::pair<const std::string, std::string> element)->void {

				lua.pushStack(element.first);
				lua.pushStack(element.second);
				lua.setTable();
			});
		}
		/*
		void LuaMap::fromLua(LuaState& lua) {
			_values.clear();

			lua.foreachKey([&](const std::string key, LuaState& state)->void {
				_values[key] = state;
			});
		}
		*/

		void LuaMap::regAll() {


		}
		 




} //end namespace core