#ifndef CORE_SDL_WRAPPED_EVENT_HPP
#define CORE_SDL_WRAPPED_EVENT_HPP


#include "Event.hpp"
#include "LuaUtil.hpp"


namespace core {



	class WrappedSdlEvent : public Event<WrappedSdlEvent> {

	public:

		SDL_Event* _wrappedEvent;

		static std::string getEventTypeNameImpl() {
			return "WrappedSdlEvent";
		}

	};




} //end namespace core



#endif