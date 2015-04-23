#ifndef CORE_LUA_MAP_HPP
#define CORE_LUA_MAP_HPP

#include "LuaTable.hpp"

namespace core {

	class LuaState;

	class LuaMap : public LuaTable {

	public:

		LuaMap& operator=(std::map<std::string, std::string> map);

		void erase(std::string key);

		std::string& operator[](const char* key);

		virtual void toLua(LuaState& lua) const;

	protected:

		std::map<std::string, std::string> _values;

	private: 

		void regAll();
	};


} //end namespace core

#endif