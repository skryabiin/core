#ifndef CORE_KEYBOARD_EVENT_HPP
#define CORE_KEYBOARD_EVENT_HPP

#include <SDL.h>
#include "Event.hpp"

namespace core {


	struct KeyboardEvent : public Event<KeyboardEvent> {


		KeyboardEvent() {
			lua_reg("keysym", &keysym);
			lua_reg("scancode", &scancode);
			lua_reg("down", &down);
			lua_reg("repeatCount", &repeatCount);
		}

		KeyboardEvent(SDL_Event& e) 
		{

			lua_reg("keysym", &keysym);
			lua_reg("scancode", &scancode);
			lua_reg("down", &down);
			lua_reg("repeatCount", &repeatCount);
			keysym = e.key.keysym.sym;
			scancode = e.key.keysym.scancode;
			down = (e.type == SDL_KEYDOWN);
			repeatCount = e.key.repeat;
		}

		int keysym;
		int scancode;
		bool down;
		int repeatCount;

		static std::string getEventTypeNameImpl() {
			return "KeyboardEvent";
		}

	};


} //end namespace core

#endif