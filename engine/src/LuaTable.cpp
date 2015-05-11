#include "LuaTable.hpp"
#include "LuaState.hpp"
#include "Console.hpp"

namespace core {

		LuaTable::LuaTable() {};

		
		LuaTable::LuaTable(const LuaTable&& other) {
			*this = std::move(other);
		}

		LuaTable& LuaTable::operator=(LuaTable&& other) {

			return other;
		}

		LuaTable& LuaTable::operator=(LuaState& state) {
			fromLua(state);
			state.popStackAdditions();
			return *this;
		}

		void LuaTable::clear() {
			intKeys.clear();
			types.clear();
			values.clear();
		}

		
		void LuaTable::erase(int index) {
			bool found = false;
			auto it = std::begin(intKeys);
			while (it != std::end(intKeys)) {

				if (it->first == index && !found) {
					auto valuesKey = it->second;
					it = intKeys.erase(it);
					values.erase(std::begin(values) + valuesKey);
					types.erase(std::begin(types) + valuesKey);
					found = true;
					continue;
				}
				else if(!found) {
					++it;
				}

				if (found) {
					int newFirst = it->first - 1;
					int newSecond = it->second - 1;
					it = intKeys.erase(it);
					intKeys[newFirst] = newSecond;
				}
			}

		}

		void LuaTable::erase(std::string key) {
			bool found = false;
			for (auto it = std::begin(stringKeys); it != std::end(stringKeys); ++it) {
				if (found) {
					--it->second;
				}
				if (it->first == key) {
					auto valuesKey = it->second;
					stringKeys.erase(it);
					values.erase(std::begin(values) + valuesKey);
					types.erase(std::begin(types) + valuesKey);
					found = true;
				}
			}
		}

		LuaTable& LuaTable::operator[](int key) {
			auto valuesKey = intKeys[key];
			return indexOpOverload(valuesKey);
		}

		LuaTable& LuaTable::operator[](const char* key) {
			auto valuesKey = stringKeys[key];
			return indexOpOverload(valuesKey);
		}

		bool LuaTable::isNil(int key) {
			return intKeys.find(key) == intKeys.end();				
		}

		bool LuaTable::isNil(const char* key) {
			return stringKeys.find(key) == stringKeys.end();							
		}

		LuaTable::operator long() const {
			return *(static_cast<long*>(value));
		}

		LuaTable::operator short() const {
			return *(static_cast<short*>(value));
		}

		LuaTable::operator unsigned short() const {
			return *(static_cast<unsigned short*>(value));
		}

		LuaTable::operator int() const {
			return *(static_cast<int*>(value));
		}

		LuaTable::operator unsigned int() const {
			return *(static_cast<unsigned int*>(value));
		}

		LuaTable::operator float() const {
			return *(static_cast<float*>(value));
		}

		LuaTable::operator bool() const {
			return *(static_cast<bool*>(value));
		}

		LuaTable::operator std::string() const {
			return *(static_cast<std::string*>(value));
		}

		LuaTable::operator LuaFunction() const {
			return *(static_cast<LuaFunction*>(value));
		}

		void LuaTable::toLua(LuaState& lua) const {
			lua.createTable(intKeys.size(), stringKeys.size());

			std::for_each(std::begin(intKeys), std::end(intKeys),
				[&](std::pair<const int, unsigned> intKey)->void {

				lua.pushStack(intKey.first);

				pushAnyValue(lua, types[intKey.second], values[intKey.second]);

				lua.setTable();

			});

			std::for_each(std::begin(stringKeys), std::end(stringKeys),
				[&](std::pair<const std::string, unsigned> stringKey)->void {

				lua.pushStack(stringKey.first);

				pushAnyValue(lua, types[stringKey.second], values[stringKey.second]);

				lua.setTable();

			});

		}

		void LuaTable::pushAnyValue(LuaState& lua, std::type_index type, void* value) const {

			 if (type == typeid(int)) {
				lua.pushStack(*static_cast<int*>(value));
			}
			else if (type == typeid(std::string)) {
				lua.pushStack(*static_cast<std::string*>(value));
			}
			else if (type == typeid(long)) {
				lua.pushStack(*static_cast<long*>(value));
			}
			else if (type == typeid(bool)) {
				lua.pushStack(*static_cast<bool*>(value));
			}
			else if (type == typeid(float)) {
				lua.pushStack(*static_cast<float*>(value));
			}
			else if (type == typeid(double)) {
				lua.pushStack(*static_cast<double*>(value));
			}
			else if (type == typeid(LuaFunction)) {
				lua.pushStack(*static_cast<LuaFunction*>(value));
			}
			else {
				auto vcast = static_cast<LuaTable*>(value);
				vcast->toLua(lua);
			}

		}

		void LuaTable::pullAnyValue(LuaState& lua, std::type_index type, void *value) {

			
			if (type == typeid(int)) {
				*static_cast<int*>(value) = lua.pullStack<int>();
			}
			else if (type == typeid(unsigned int)) {
				*static_cast<unsigned int*>(value) = lua.pullStack<unsigned int>();
			}
			else if (type == typeid(short)) {
				*static_cast<short*>(value) = lua.pullStack<short>();
			}
			else if (type == typeid(unsigned short)) {
				*static_cast<unsigned short*>(value) = lua.pullStack<unsigned short>();
			}
			else if (type == typeid(long)) {
				*static_cast<long*>(value) = lua.pullStack<long>();
			}
			else if (type == typeid(std::string)) {
				*static_cast<std::string*>(value) = lua.pullStack<std::string>();
			}
			else if (type == typeid(float)) {
				*static_cast<float*>(value) = lua.pullStack<float>();
			}
			else if (type == typeid(double)) {
				*static_cast<double*>(value) = lua.pullStack<double>();
			}
			else if (type == typeid(bool)) {
				*static_cast<bool*>(value) = lua.pullStack<bool>();
			}
			else if (type == typeid(LuaFunction)) {
				*static_cast<LuaFunction*>(value) = lua.pullStack<LuaFunction>();
			}
			else {
				static_cast<LuaTable*>(value)->fromLua(lua);
			}
				 
		}

		void LuaTable::fromLua(LuaState& lua) {
			
			std::for_each(std::begin(intKeys), std::end(intKeys),
				[&](std::pair<const int, unsigned> intKey)->void {

				int x = 0;

				lua.getTable(intKey.first);

				pullAnyValue(lua, types[intKey.second], values[intKey.second]);

				lua.popStack();

			});

			std::for_each(std::begin(stringKeys), std::end(stringKeys),
				[&](std::pair<const std::string, unsigned> stringKey)->void {

				lua.getTable(stringKey.first);

				pullAnyValue(lua, types[stringKey.second], values[stringKey.second]);

				lua.popStack();

			});

		}


		int LuaTable::size() const {

			return values.size();
		}

		int LuaTable::stringKeySize() const {
			return stringKeys.size();
		}

		int LuaTable::intKeySize() const {
			return intKeys.size();
		}

	
		LuaTable& LuaTable::indexOpOverload(unsigned valuesKey) {
			if (types[valuesKey] == typeid(LuaTable)) {
				return *(static_cast<LuaTable*>(values[valuesKey]));
			}
			auto& out = LuaTable{};
			out.value = values[valuesKey];
			return out;

		}

} //end namespace core