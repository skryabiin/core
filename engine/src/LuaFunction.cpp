#include "LuaFunction.hpp"
#include "LuaState.hpp"
#include "Core.hpp"

namespace core {

	LuaFunction::LuaFunction() : _callbackRef{ LUA_REFNIL } {};

	LuaFunction::~LuaFunction() {
		//single<Core>().lua().removeReference(_callbackRef);
	}

	int LuaFunction::getCallbackRef() {
		return _callbackRef;
	}
}