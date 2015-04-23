#include "MouseEvent.hpp"

namespace core {

	MouseEvent::MouseEvent() {
		lua_reg("m1down", &m1down);
		lua_reg("m2down", &m2down);
		lua_reg("m3down", &m3down);
		lua_reg("m4down", &m4down);
		lua_reg("m5down", &m5down);
		lua_reg("m1downOld", &m1downOld);
		lua_reg("m2downOld", &m2downOld);
		lua_reg("m3downOld", &m3downOld);
		lua_reg("m4downOld", &m4downOld);
		lua_reg("m5downOld", &m5downOld);
		lua_reg("mdown", &mdown);
		lua_reg("mdownOld", &mdown);
		lua_reg("position", &position);
		lua_reg("delta", &delta);
	}


	MouseEvent::MouseEvent(SDL_Event& e) {
		lua_reg("mdown", &mdown);

		lua_reg("m1down", &m1down);
		lua_reg("m2down", &m2down);
		lua_reg("m3down", &m3down);
		lua_reg("m4down", &m4down);
		lua_reg("m5down", &m5down);
		lua_reg("position", &position);
		lua_reg("delta", &delta);

		position.setPixel(Pixel(e.motion.x, e.motion.y));
		delta[0] = e.motion.xrel;
		delta[1] = e.motion.yrel;


		m1down = !!(e.motion.state & SDL_BUTTON_LMASK);
		m2down = !!(e.motion.state & SDL_BUTTON_MMASK);
		m3down = !!(e.motion.state & SDL_BUTTON_RMASK);
		m4down = !!(e.motion.state & SDL_BUTTON_X1MASK);
		m5down = !!(e.motion.state & SDL_BUTTON_X2MASK);

		mdown = m1down || m2down || m3down || m4down || m5down;
	}

	void MouseEvent::setKeyStates(int newKeyStates) {
		oldKeyStates = keyStates;
		keyStates = newKeyStates;

		m1downOld = m1down;
		m2downOld = m2down;
		m3downOld = m3down;
		m4downOld = m4down;
		m5downOld = m5down;
		mdownOld = mdown;

		m1down = !!(newKeyStates & SDL_BUTTON_LMASK);
		m2down = !!(newKeyStates & SDL_BUTTON_MMASK);
		m3down = !!(newKeyStates & SDL_BUTTON_RMASK);
		m4down = !!(newKeyStates & SDL_BUTTON_X1MASK);
		m5down = !!(newKeyStates & SDL_BUTTON_X2MASK);

		mdown = m1down || m2down || m3down || m4down || m5down;
	}


	std::string MouseEvent::getEventTypeNameImpl() {
		return "MouseEvent";
	}

}