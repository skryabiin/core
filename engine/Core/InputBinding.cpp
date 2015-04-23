#include <cstring>
#include <functional>
#include "InputBinding.h"
#include "Templates.hpp"
#include "Console.h"
#include "PhysicsSystem"
#include "ColorState.h"
#include "Core.h"
#include "World.h"
#include "AnimationSystem.h"
#include "Animation.h"
#include "LuaBind.h"

namespace core {

	bool InputBinding::init() {

		std::function<void(InputBinding*, WrappedSdlEvent&)> sdlEventCallback = std::mem_fn(&InputBinding::handleSdlEvent);
		_eventFilter.init(this, sdlEventCallback);
		return true;
	}

	void InputBinding::handleSdlEvent(WrappedSdlEvent& event) {
		if (_paused) return; //TODO this functionality needs to be moved up somewhere

		int type = event._wrappedEvent->type;
		
		if (event._wrappedEvent->type == SDL_CONTROLLERBUTTONDOWN) {
			_currentGamepadStates[event._wrappedEvent->cbutton.button] = 1;
			int button = event._wrappedEvent->cbutton.button;
			single<Console>() << "Button down: " << button << "\n";
		}
		else if (event._wrappedEvent->type == SDL_CONTROLLERBUTTONUP) {
			_currentGamepadStates[event._wrappedEvent->cbutton.button] = 0;
			int button = event._wrappedEvent->cbutton.button;
			single<Console>() << "Button up: " << button << "\n";
		}
		if (event._wrappedEvent->type == SDL_KEYDOWN &&  event._wrappedEvent->key.repeat == 0) {
			_currentKeyStates[event._wrappedEvent->key.keysym.scancode] = 1;
			if (event._wrappedEvent->key.keysym.sym == SDLK_BACKSPACE && single<Console>()) {
				printStates("Curr gmpad", _currentGamepadStates);
				printStates("Prev gmpad", _previousGamepadStates);
				printStates("Curr kybrd", _currentKeyStates);
				printStates("Prev kybrd", _previousKeyStates);
			}

		}
		else if (event._wrappedEvent->type == SDL_KEYUP) {
			_currentKeyStates[event._wrappedEvent->key.keysym.scancode] = 0;
		}
	}

	void InputBinding::updatePreviousInputStates() {
		for (int i = 0; i < 512; i++) {
			_previousKeyStates[i] = _currentKeyStates[i];
			_previousGamepadStates[i] = _currentGamepadStates[i];
		}
	}
	void InputBinding::updateImpl(RuntimeContext& context) {
		for (auto& facet : _inputFacets) {

			if ((keyStateDisposition(facet.config.keyScancodeLeft) == KeyStateDisposition::STILL_DOWN) ||
				(buttonStateDisposition(facet.config.buttonLeft) == ButtonStateDisposition::STILL_DOWN))

			{

				auto ace = AnimationChangeEvent{};
				ace.entity = facet.of();
				ace.nextAnimation = "MoveLeft";
				ace.endImmediate = true;
				single<EventProcessor>().process(ace);

				auto vce = VelocityChangeEvent{};
				vce.entity = facet.of();
				vce.axes.value = -1; //x only
				vce.additive.value = false;
				vce.velocityChange = Vec2{ -1.0f * facet.config.moveSpeed, 0.0f };
				single<EventProcessor>().process(vce);
				/*
				if (_of->findAspect<ld::MoveLeftAction>("MoveLeftAction")->tryAction()) {
				_of->findAspect<ld::MoveRightAction>("MoveRightAction")->pause();
				_of->findAspect<ld::StandLeftAction>("StandLeftAction")->pause();
				_of->findAspect<ld::StandRightAction>("StandRightAction")->pause();
				}
				*/
			}

			if ((keyStateDisposition(facet.config.keyScancodeLeft) == KeyStateDisposition::NOW_UP) ||
				(buttonStateDisposition(facet.config.buttonLeft) == ButtonStateDisposition::NOW_UP))

			{

				auto ace = AnimationChangeEvent{};
				ace.entity = facet.of();
				ace.nextAnimation = "StandLeft";
				ace.endImmediate = true;
				single<EventProcessor>().process(ace);

				auto vce = VelocityChangeEvent{};
				vce.entity = facet.of();
				vce.axes.value = -1; //x only
				vce.additive.value = false;
				vce.velocityChange = Vec2{ 0.0f, 0.0f };
				single<EventProcessor>().process(vce);
				/*
				_of->findAspect<ld::MoveLeftAction>("MoveLeftAction")->pause();
				_of->findAspect<ld::StandLeftAction>("StandLeftAction")->resume();
				*/
			}

			if ((keyStateDisposition(facet.config.keyScancodeRight) == KeyStateDisposition::STILL_DOWN) ||
				(buttonStateDisposition(facet.config.buttonRight) == ButtonStateDisposition::STILL_DOWN))
			{

				auto ace = AnimationChangeEvent{};
				ace.entity = facet.of();
				ace.nextAnimation = "MoveRight";
				ace.endImmediate = true;
				single<EventProcessor>().process(ace);

				auto vce = VelocityChangeEvent{};
				vce.entity = facet.of();
				vce.axes.value = -1; //x only
				vce.additive.value = false;
				vce.velocityChange = Vec2{ 1.0f * facet.config.moveSpeed, 0.0f };
				single<EventProcessor>().process(vce);

				/*
				if (_of->findAspect<ld::MoveRightAction>("MoveRightAction")->tryAction()) {
				_of->findAspect<ld::MoveLeftAction>("MoveLeftAction")->pause();
				_of->findAspect<ld::StandLeftAction>("StandLeftAction")->pause();
				_of->findAspect<ld::StandRightAction>("StandRightAction")->pause();
				} */
			}

			if ((keyStateDisposition(facet.config.keyScancodeRight) == KeyStateDisposition::NOW_UP) ||
				(buttonStateDisposition(facet.config.buttonRight) == ButtonStateDisposition::NOW_UP))

			{
				auto ace = AnimationChangeEvent{};
				ace.entity = facet.of();
				ace.nextAnimation = "StandRight";
				ace.endImmediate = true;
				single<EventProcessor>().process(ace);

				auto vce = VelocityChangeEvent{};
				vce.entity = facet.of();
				vce.axes.value = -1; //x only
				vce.additive.value = false;
				vce.velocityChange = Vec2{ 0.0f, 0.0f };
				single<EventProcessor>().process(vce);
				/*
				_of->findAspect<ld::MoveRightAction>("MoveRightAction")->pause();
				_of->findAspect<ld::StandRightAction>("StandRightAction")->resume();
				*/
			}

			if ((keyStateDisposition(facet.config.keyScancodeJump) == KeyStateDisposition::NOW_DOWN) ||
				(buttonStateDisposition(facet.config.buttonJump) == ButtonStateDisposition::NOW_DOWN)) {

				auto vce = VelocityChangeEvent{};
				vce.entity = facet.of();
				vce.axes.value = 1; //y only
				vce.additive.value = false;
				vce.velocityChange = Vec2{ 0.0f, 1.0f * facet.config.jumpSpeed };
				single<EventProcessor>().process(vce);
				auto jumpSound = single<ResourceManager>().getSound("Jump");
				if (jumpSound != nullptr) {
					jumpSound->play();
				}


			}


			//cycle colors
			if ((keyStateDisposition(facet.config.keyScancodeCycleColorPrev) == KeyStateDisposition::NOW_DOWN) ||
				(buttonStateDisposition(facet.config.buttonCycleColorPrev) == ButtonStateDisposition::NOW_DOWN)) {
				auto cycleDown = ColorCycleEvent{};
				cycleDown.increment = -1;				
				single<EventProcessor>().process(cycleDown);
				
			}

			if ((keyStateDisposition(facet.config.keyScancodeCycleColorNext) == KeyStateDisposition::NOW_DOWN) ||
				(buttonStateDisposition(facet.config.buttonCycleColorNext) == ButtonStateDisposition::NOW_DOWN)) {
				auto cycleUp = ColorCycleEvent{};
				cycleUp.increment = 1;
				single<EventProcessor>().process(cycleUp);
			}

			if ((keyStateDisposition(facet.config.keyScancodeUp) == KeyStateDisposition::NOW_DOWN) ||
				(buttonStateDisposition(facet.config.buttonUp) == ButtonStateDisposition::NOW_DOWN)) {


				//testing text render system position placement
				auto posce = PositionChangeEvent{};
				posce.entity = Entity{ 412 };
				posce.position = Pixel{ 200, 200, 0 };
				single<EventProcessor>().process(posce);

				auto textChange = TextContentChange{};
				textChange.entity = posce.entity;
				textChange.textContent = "Testing change of text";
				single<EventProcessor>().process(textChange);

				//accessing doors goes here
				auto query = ObjectCollisionQuery{};
				query.concerning = facet.of();
				single<EventProcessor>().process(query);
				
				if (!query.contacts.empty()) {
					DoorFacet* doorFacet = nullptr;

					auto* worldSystem = single<Core>().getSystemByName<World>("TmxWorld");
					auto* physicsSystem = single<Core>().getSystemByName<PhysicsSystem>("Layer1Physics"); //TODO this needs to be the system that the associated entity is in
					for (unsigned i = 0; i < query.contacts.size(); i++) {
						doorFacet = worldSystem->getDoorFacet(query.contacts[i]);

						if (doorFacet != nullptr) {
							auto linkedDoor = worldSystem->getDoorFacet(doorFacet->linkedDoorId);
							auto physicalLocation = single<Core>().getFacetsByType<PhysicsFacet>(linkedDoor->of());

							if (!physicalLocation.empty()) {
								auto newPosition = physicalLocation[0]->p;
								newPosition.y = newPosition.y - physicalLocation[0]->h;
								physicsSystem->setPosition(facet.of(), newPosition);

								auto doorSound = single<ResourceManager>().getSound("DoorOpen");
								if (doorSound != nullptr) {
									doorSound->play();
								}

								break;
							}

						}
					}
				}

			}

		}
		updatePreviousInputStates();
	}


	KeyStateDisposition InputBinding::keyStateDisposition(int scancode) {
		if (_currentKeyStates[scancode] == _previousKeyStates[scancode]) {
			return (_currentKeyStates[scancode]) ? KeyStateDisposition::STILL_DOWN : KeyStateDisposition::STILL_UP;
		}
		else if (_currentKeyStates[scancode]) {
			return KeyStateDisposition::NOW_DOWN;
		}
		else {
			return KeyStateDisposition::NOW_UP;
		}
	}
	ButtonStateDisposition InputBinding::buttonStateDisposition(int button) {
		if (_currentGamepadStates[button] == _previousGamepadStates[button]) {
			return (_currentGamepadStates[button]) ? ButtonStateDisposition::STILL_DOWN : ButtonStateDisposition::STILL_UP;
		}
		else if (_currentGamepadStates[button]) {
			return ButtonStateDisposition::NOW_DOWN;
		}
		else {
			return ButtonStateDisposition::NOW_UP;
		}

	}


	InputFacet& InputBinding::createInputFacet(Entity& e) {

		auto facet = InputFacet{};
		facet.setOf(e);

		facet.config.keyScancodeLeft = luaGlobalVar<int>(single<Core>().L(), "keyboardScancodeLeft");
		facet.config.keyScancodeRight = luaGlobalVar<int>(single<Core>().L(), "keyboardScancodeRight");
		facet.config.keyScancodeJump = luaGlobalVar<int>(single<Core>().L(), "keyboardScancodeJump");
		facet.config.keyScancodeUp = luaGlobalVar<int>(single<Core>().L(), "keyboardScancodeUp");
		facet.config.keyScancodeDown = luaGlobalVar<int>(single<Core>().L(), "keyboardScancodeDown");
		facet.config.keyScancodeCycleColorPrev = luaGlobalVar<int>(single<Core>().L(), "keyboardScancodeCycleColorPrev");
		facet.config.keyScancodeCycleColorNext = luaGlobalVar<int>(single<Core>().L(), "keyboardScancodeCycleColorNext");
		facet.config.keyboardActive = luaGlobalVar<bool>(single<Core>().L(), "keyboardActive");

		facet.config.buttonLeft = luaGlobalVar<int>(single<Core>().L(), "gamepadButtonLeft");
		facet.config.buttonRight = luaGlobalVar<int>(single<Core>().L(), "gamepadButtonRight");
		facet.config.buttonJump = luaGlobalVar<int>(single<Core>().L(), "gamepadButtonJump");
		facet.config.buttonUp = luaGlobalVar<int>(single<Core>().L(), "gamepadButtonUp");
		facet.config.buttonDown = luaGlobalVar<int>(single<Core>().L(), "gamepadButtonDown");
		facet.config.buttonCycleColorPrev = luaGlobalVar<int>(single<Core>().L(), "gamepadButtonCycleColorPrev");
		facet.config.buttonCycleColorNext = luaGlobalVar<int>(single<Core>().L(), "gamepadButtonCycleColorNext");

		facet.config.gamepadActive = luaGlobalVar<bool>(single<Core>().L(), "gamepadActive");

		//the standard move speed = 
		facet.config.moveSpeed = luaGlobalVar<int>(single<Core>().L(), "stats.player.moveSpeed");
		facet.config.jumpSpeed = luaGlobalVar<float>(single<Core>().L(), "stats.player.jumpSpeed");

		//the amount of time it takes to reach full movement speed, and to reach no movement when stopped
		//not implemented yet
		//facet.config.moveAccelDuration = single<Core>().globalVar<float>(
		//facet.config.moveDecelDuration = single<Core>().globalVar<float>(


		_inputFacets.push_back(facet);
		return _inputFacets.back();

	}


	void InputBinding::printStates(std::string name, std::array<int, 512> arr) {

		single<Console>() << name << " states: ";
		for (int i = 0; i < 512; i++) {
			single<Console>() << arr[i];
		}
		single<Console>() << "\n";

	}

} //end namespace core