#include "Config.hpp"
#include "Core.hpp"

namespace core {


	bool Config::reload() {
		auto& lua = single<Core>().lua();
		lua.global("Config");
		fromLua(lua);
		return true;
	}

} //end namespace core