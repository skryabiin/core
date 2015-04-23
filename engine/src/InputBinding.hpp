#ifndef CORE_INPUT_BINDING_HPP
#define CORE_INPUT_BINDING_HPP


#include <algorithm>
#include <array>

#include "Entity.hpp"
#include "Facet.hpp"
#include "System.hpp"
#include "Event.hpp"
#include "RuntimeContext.hpp"
#include "Config.hpp"

namespace core {
	

	enum class KeyStateDisposition {
		NOW_UP,
		NOW_DOWN,
		STILL_UP,
		STILL_DOWN
	};

	enum class ButtonStateDisposition {
		NOW_UP,
		NOW_DOWN,
		STILL_UP,
		STILL_DOWN
	};


	struct InputConfig {

			int keyScancodeLeft;
			int keyScancodeRight;
			int keyScancodeJump;
			int keyScancodeUp;
			int keyScancodeDown;
			int keyScancodeCycleColorPrev;
			int keyScancodeCycleColorNext;

			int buttonLeft;
			int buttonRight;
			int buttonJump;
			int buttonUp;
			int buttonDown;
			int buttonCycleColorPrev;
			int buttonCycleColorNext;

			bool keyboardActive;
			bool gamepadActive;

			//the standard move speed
			float moveSpeed;

			//the amount of time it takes to reach full movement speed, and to reach no movement when stopped
			//not implemented yet
			float moveAccelDuration;
			float moveDecelDuration;

			float jumpSpeed;
	};

	struct InputFacet : public Facet {
		InputConfig config;
	};


	class InputBinding : public UpdateableSystem,  public initializable<InputBinding, std::string> {

	public:

		InputBinding()  {
			std::fill(begin(_currentKeyStates), end(_currentKeyStates), 0);
			std::fill(begin(_previousKeyStates), end(_previousKeyStates), 0);
			std::fill(begin(_currentGamepadStates), end(_currentGamepadStates), 0);
			std::fill(begin(_previousGamepadStates), end(_previousGamepadStates), 0);

		};

		bool init();

		void handleSdlEvent(WrappedSdlEvent& event);

		void updatePreviousInputStates();
		void updateImpl(RuntimeContext& context);

		KeyStateDisposition keyStateDisposition(int scancode);
		ButtonStateDisposition buttonStateDisposition(int button);

		InputFacet& createInputFacet(Entity& e);

		virtual void cleanup() {};
	private:

		EventFilter<WrappedSdlEvent> _eventFilter;
		std::array<int, 512> _currentKeyStates;
		std::array<int, 512> _previousKeyStates;
		std::array<int, 512> _currentGamepadStates;
		std::array<int, 512> _previousGamepadStates;

		void printStates(std::string name, std::array<int, 512> arr);

		std::vector<InputFacet> _inputFacets;

	};


} //end namespace core


#endif