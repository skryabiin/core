#ifndef CORE_GAMEPAD_EVENT_HPP
#define CORE_GAMEPAD_EVENT_HPP

#include <SDL.h>
#include "Event.hpp"

namespace core {

	struct GamepadEvent : public Event<GamepadEvent>  {

		GamepadEvent() {
		
			lua_reg("button", &button);
			lua_reg("down", &down);		
		}
		
		
		GamepadEvent(SDL_Event& e)		
		{
			lua_reg("button", &button);
			lua_reg("down", &down);
			button = e.cbutton.button;
			down = (e.type == SDL_CONTROLLERBUTTONDOWN);
		}

		
		int button;
		bool down;


		static std::string getEventTypeNameImpl() {
			return "GamepadEvent";
		}

	};


} //end namespace core

#endif