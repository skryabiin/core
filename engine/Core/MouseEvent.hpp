#ifndef CORE_MOUSE_EVENT_HPP
#define CORE_MOUSE_EVENT_HPP

#include "LuaVector.hpp"
#include "Event.hpp"
#include "Math.hpp"


namespace core {


	struct MouseEvent : public Event<MouseEvent> {

		MouseEvent();

		MouseEvent(SDL_Event& e);

		void setKeyStates(int newKeyStates);

		int oldKeyStates;
		int keyStates;
		bool m1down;
		bool m2down;
		bool m3down;
		bool m4down;
		bool m5down;
		bool m1downOld;
		bool m2downOld;
		bool m3downOld;
		bool m4downOld;
		bool m5downOld;
		bool mdown;
		bool mdownOld;

		LuaPixel position;		
		LuaVec2 delta;


		static std::string getEventTypeNameImpl();

	};


} //end namespace core

#endif