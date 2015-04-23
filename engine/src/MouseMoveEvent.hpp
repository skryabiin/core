#ifndef CORE_MOUSE_MOVE_EVENT_HPP
#define CORE_MOUSE_MOVE_EVENT_HPP

#include "Event.hpp"
#include "LuaVector.hpp"

namespace core {


	struct MouseMoveEvent : public Event<MouseMoveEvent> {


		MouseMoveEvent() {
			lua_reg("windowId", &windowId);
			lua_reg("m1down", &m1down);
			lua_reg("m2down", &m2down);
			lua_reg("m3down", &m3down);
			lua_reg("m4down", &m4down);
			lua_reg("m5down", &m5down);
			lua_reg("position", &position);
			lua_reg("delta", &delta);
		}


		MouseMoveEvent(SDL_Event& e) {
			lua_reg("windowId", &windowId);
			lua_reg("m1down", &m1down);
			lua_reg("m2down", &m2down);
			lua_reg("m3down", &m3down);
			lua_reg("m4down", &m4down);
			lua_reg("m5down", &m5down);
			lua_reg("position", &position);
			lua_reg("delta", &delta);

			windowId = e.motion.windowID;
			
			position.setPixel(Pixel(e.motion.x, e.motion.y));
			delta[0] = e.motion.xrel;
			delta[1] = e.motion.yrel;

			m1down = !!(e.motion.state & SDL_BUTTON_LMASK);
			m2down = !!(e.motion.state & SDL_BUTTON_MMASK);
			m3down = !!(e.motion.state & SDL_BUTTON_RMASK);
			m4down = !!(e.motion.state & SDL_BUTTON_X1MASK);
			m5down = !!(e.motion.state & SDL_BUTTON_X2MASK);

		}

		int windowId;
		bool m1down;
		bool m2down;
		bool m3down;
		bool m4down;
		bool m5down;
		LuaPixel position;
		LuaVec2 delta;		


		static std::string getEventTypeNameImpl() {
			return "MouseMoveEvent";
		}

	};



} //end namespace core

#endif