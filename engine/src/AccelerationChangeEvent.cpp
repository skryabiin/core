#include "AccelerationChangeEvent.hpp"

namespace core {


	AccelerationChangeEvent::AccelerationChangeEvent()  {

		lua_reg("additive", &additive);
		lua_reg("axes", &axes);
		lua_reg("entityId", &entity);
	}




	std::string AccelerationChangeEvent::getEventTypeNameImpl() {

		return "AccelerationChangeEvent";
	}

	bool AccelerationChangeEvent::createFromLua(LuaState& lua) {

		auto fdq = AccelerationChangeEvent{};

		fdq.fromLua(lua);

		single<EventProcessor>().process(fdq);

		return false;


	}

} //end namespace core