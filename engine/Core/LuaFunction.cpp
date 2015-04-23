#include "LuaFunction.hpp"
#include "LuaState.hpp"
#include "Core.hpp"

namespace core {

	void LuaFunction::cleanup() {
		single<Core>().lua().removeReference(_callbackRef);
	}
}