#ifndef CORE_MOUSE_BUTTON_EVENT_HPP
#define CORE_MOUSE_BUTTON_EVENT_HPP

#include "Event.hpp"
#include "LuaVector.hpp"

namespace core  {


	struct MouseButtonEvent : public Event<MouseButtonEvent> {



		MouseButtonEvent() {
			lua_reg("windowId", &windowId);
			lua_reg("down", &down);
			lua_reg("button", &button);
			lua_reg("position", &position);
		}

		MouseButtonEvent(SDL_Event& e) {
			lua_reg("windowId", &windowId);
			lua_reg("down", &down);
			lua_reg("button", &button);
			lua_reg("position", &position);

			windowId = e.button.windowID;
			down = (e.button.state == SDL_PRESSED);
			button = e.button.button;
			position.setPixel(Pixel(e.button.x, e.button.y));
		}

		int windowId;
		bool down;
		int button;
		LuaPixel position;


		static std::string getEventTypeNameImpl() {
			return "MouseButtonEvent";
		}

	};

} //end namespace core

#endif