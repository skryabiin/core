#ifndef CORE_LUA_FUNCTION_HPP
#define CORE_LUA_FUNCTION_HPP

namespace core {


	class LuaFunction {
		friend class LuaState;

	public:

		LuaFunction();

		~LuaFunction();

		int getCallbackRef();

	private:

		int _callbackRef;

	};



} //end namespace core

#endif