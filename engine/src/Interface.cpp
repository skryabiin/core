#include "Interface.hpp"
#include "Core.hpp"
#include "TextureRenderSystem2d.hpp"
#include "EntitiesInRectQuery.hpp"
#include "InterfaceFacet.hpp"
#include "EntityPositionQuery.hpp"
#include "EntityLayerQuery.hpp"
#include "KeyboardEvent.hpp"
#include "GamepadEvent.hpp"

namespace core {

	Interface::Interface() {
		_sdlGamepad = nullptr;

	}

	InitStatus Interface::initializeImpl() {

		this->System::initializeImpl();
		
		
		debug("in interface init.");


		SDL_Init(SDL_INIT_GAMECONTROLLER);
		checkGamepadStatus();

		_textureRenderSystem = new TextureRenderSystem2d{};
		_textureRenderSystem->setName("InterfaceTextures");
		single<Core>().addSystem(std::unique_ptr<TextureRenderSystem2d>(_textureRenderSystem));
		single<Core>().includeRenderableSystem2d(_textureRenderSystem);
		_textureRenderSystem->initialize();
		_textureRenderSystem->setDrawableLayerId(100);
		_mouseCursor = single<Core>().createEntity();
		setCursorTextureFromDef();

		auto& lua = single<Core>().lua();
		lua.bindFunction("getMouseState_bind", Interface::getMouseState_bind);
		lua.bindFunction("setCursorTexture_bind", Interface::setCursorTexture_bind);
		lua.bindFunction("updateInterfaceFacet_bind", Interface::updateInterfaceFacet_bind);
		lua.bindFunction("getKeyStates_bind", Interface::getKeyStates_bind);
		lua.bindFunction("getGamepadStates_bind", Interface::getGamepadStates_bind);

		return InitStatus::INIT_TRUE;
	}

	InitStatus Interface::resetImpl() {

		_facets.clear();
		_facets.shrink_to_fit();

		return InitStatus::INIT_FALSE;
	}


	//poll sdl for interface events
	void Interface::pollSdlEvents() {



		auto& ep = single<EventProcessor>();
		while (true)
		{

			SDL_Event e{};
			if (SDL_PollEvent(&e) != 0) {
				


				if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
					auto keyboardEvent = KeyboardEvent{ e };
					ep.process(keyboardEvent);
				}
				else if (e.type == SDL_CONTROLLERBUTTONDOWN || e.type == SDL_CONTROLLERBUTTONUP) {
					auto gamepadEvent = GamepadEvent{ e };
					ep.process(gamepadEvent);
				}
				else if (e.type != SDL_MOUSEBUTTONDOWN && e.type != SDL_MOUSEBUTTONUP && e.type != SDL_MOUSEMOTION) {
					WrappedSdlEvent sdlEvent{};
					sdlEvent._wrappedEvent = &e;
					single<EventProcessor>().process(sdlEvent);
				}
				else if (e.type == SDL_QUIT) {
					WrappedSdlEvent sdlEvent{};
					sdlEvent._wrappedEvent = &e;
					single<EventProcessor>().process(sdlEvent);
				}
			}
			else {
				break;
			}
		}

	}



	void Interface::updateImpl(RuntimeContext& runtimeContext) {

		pollSdlEvents();
		updateMouseState();

		auto p = _mouseState.position.getPixel();

	}





	void Interface::updateMouseState() {

		auto pos = Pixel{};
		auto keyStates = SDL_GetMouseState(&pos.x, &pos.y);
		auto& lua = single<Core>().lua();
		int winHeight = lua("Config")["window"][2];
		pos.y = winHeight - pos.y - 1;
		if (!(_mouseState.position.getPixel() == pos) || _mouseState.keyStates != keyStates) {
			_mouseState.delta[0] = _mouseState.position[0] - pos.x;
			_mouseState.delta[1] = _mouseState.position[1] - pos.y;
			_mouseState.position.setPixel(pos);
			_mouseState.setKeyStates(keyStates);
			single<EventProcessor>().process(_mouseState);


			auto cursorPce = PositionChangeEvent{};
			cursorPce.entity = _mouseCursor;
			cursorPce.position = pos;
			_textureRenderSystem->updateDrawPosition(cursorPce);
			
			auto selectedLayer = 0;

			_interfaceState.currentPosition.setPixel(pos);

			//if the mouse was clicked this tick, check to see if there is something to pick up
			if (_mouseState.mdown && !_mouseState.mdownOld) {
				_interfaceState.clickPosition.setPixel(pos);
				auto entitySearchRect = SDL_Rect{ pos.x, pos.y , pos.x + 1, pos.y + 1};
				auto rectQuery = EntitiesInRectQuery{};
				rectQuery.rect = entitySearchRect;
				single<EventProcessor>().process(rectQuery);
				
				auto selectedPos = Pixel{};
				for (Entity e : rectQuery.entities.values()) {					
					for (auto& facet : _facets) {					

						if (facet.of() == e && !facet.isPaused()) {
							if (facet.clickable) {
								single<Core>().lua().call(facet.onClick, _interfaceState);
							}
							if (facet.draggable) {

								auto entityLayerQuery = EntityLayerQuery{};
								entityLayerQuery.entity = e;
								single<EventProcessor>().process(entityLayerQuery);
								if (entityLayerQuery.found && entityLayerQuery.layerId >= selectedLayer) {
									selectedLayer = entityLayerQuery.layerId;

									auto entityPositionQuery = EntityPositionQuery{};
									entityPositionQuery.entity = e;
									single<EventProcessor>().process(entityPositionQuery);
									if (entityPositionQuery.found) {
										auto entityPos = entityPositionQuery.position.getPixel();
										if (entityPos.z > selectedPos.z || _interfaceState.pickedUp == nullptr) {										
											_interfaceState.pickedUp = &facet;
											_interfaceState.pickedUpThisTick = true;
											_interfaceState.pickedUpPosition.setPixel(entityPos);
											selectedPos = entityPos;
										}
									}
								}
							}
						}
					}
				}
			}
			
			if (_interfaceState.pickedUpThisTick) {
				single<Core>().lua().call(_interfaceState.pickedUp->onDrag, _interfaceState);
				_interfaceState.pickedUpThisTick = false;
			}

			//if there is an entity picked up and mouse1 is still down, move it with the cursor
			if (_mouseState.mdown && _mouseState.mdownOld && _interfaceState.pickedUp != nullptr) {			
				single<Core>().lua().call(_interfaceState.pickedUp->onDrag, _interfaceState);
				/*
				auto pce = PositionChangeEvent{};
				pce.entity = _interfaceState.pickedUp;
				pce.position = _interfaceState.selectedPosition;
				single<EventProcessor>().process(pce);
				*/
				
			}

			//if there's something picked up and mouse1 is up, release the entity
			else if (!_mouseState.mdown && _interfaceState.pickedUp != nullptr) {
				_interfaceState.pickedUp = nullptr;						
			}

		}

	}

	void Interface::handleFacetPauseEvent(FacetPauseEvent& pauseEvent) {

		for (auto& facet : _facets) {
			if (pauseEvent.entity == facet.of()) {		
				
			if (pauseEvent.facetId == -1 || pauseEvent.facetId == facet.id()) {
				if (pauseEvent.paused) {
					facet.pause();					
				}
				else {
					facet.resume();					
				}				
			}
				break;
			}
		}
	}

	void Interface::setCursorTexture(std::string textureName, SDL_Rect sourceRect, Color textureColorMod, Dimension dimensions, Vec2 scale, Pixel offset) {
		_mouseCursorDef.textureName = textureName;
		_mouseCursorDef.sourceRect = sourceRect;
		_mouseCursorDef.textureColorMod = textureColorMod;
		_mouseCursorDef.dimensions = dimensions;
		_mouseCursorDef.scale = scale;
		_mouseCursorDef.offset = offset;
		_mouseCursorDef.isSet = true;
		setCursorTextureFromDef();


	}

	void Interface::setCursorTextureFromDef() {
		if (_mouseCursorDef.isSet == false) return;
		auto existingFacets = _textureRenderSystem->getFacets(_mouseCursor);
		if (existingFacets.empty()) {
			_textureRenderSystem->createTextureFacet(_mouseCursor, _mouseState.position.getPixel(), _mouseCursorDef.offset, _mouseCursorDef.dimensions, _mouseCursorDef.scale, _mouseCursorDef.sourceRect, _mouseCursorDef.textureName);
		}
		else {
			auto textureChangeEvent = TextureChangeEvent{};
			textureChangeEvent.entity = _mouseCursor;
			textureChangeEvent.sourceTextureRect = LuaRect{ _mouseCursorDef.sourceRect };
			textureChangeEvent.textureName = _mouseCursorDef.textureName;
			textureChangeEvent.facetId = 1;
			_textureRenderSystem->updateTexture(textureChangeEvent);

		}

	}

	int Interface::setCursorTexture_bind(LuaState& lua) {

		std::string textureName = lua["textureName"];
		LuaRect sourceRect = lua["source"];
		LuaColor textureColorMod = lua["textureColorMod"];
		LuaDimension dimensions = lua["dimensions"];
		LuaPixel offset = lua["offset"];
		LuaVec2 scale = lua["scale"];		
		single<Interface>().setCursorTexture(textureName, sourceRect.getRect(), textureColorMod.getColor(), dimensions.getDimension(), scale.getVec2(), offset.getPixel());

		return 0;
	}

	MouseEvent Interface::getMouseState() {
		return _mouseState;
	}

	int Interface::getMouseState_bind(LuaState& lua) {

		MouseEvent state = single<Interface>().getMouseState();

		state.toLua(lua);

		return 1;

	}

	InterfaceFacet* Interface::updateFacet(Entity& e, bool draggable, bool hoverable, bool clickable, LuaFunction onClick, LuaFunction onDrag) {		

		InterfaceFacet* facetToUpdate = nullptr;
		for (auto& facet : _facets) {

			if (facet.of() == e) {
				facetToUpdate = &facet;
				break;
			}
		}

		if (facetToUpdate == nullptr) {
			auto facet = InterfaceFacet{};
			facet.setOf(e);
			_facets.push_back(facet);
			facetToUpdate = &(_facets.back());
		}
		facetToUpdate->draggable = draggable;
		facetToUpdate->hoverable = hoverable;
		facetToUpdate->clickable = clickable;		
		facetToUpdate->onClick = onClick;
		facetToUpdate->onDrag = onDrag;
		return facetToUpdate;
	}
	 
	void Interface::checkGamepadStatus() {

		if (_sdlGamepad == nullptr) {
			if (SDL_NumJoysticks() > 0) {
				if (SDL_IsGameController(0)) {
					_sdlGamepad = SDL_GameControllerOpen(0);
				}
				if (_sdlGamepad == NULL) {
					singleton<Console>::instance() << "Gamepad pointer from SDL is null.\n";
				}
				else {
					singleton<Console>::instance() << "Gamepad detected!\n";
				}
			}
			else {
				singleton<Console>::instance() << "No gamepads found.\n";
			}
		}
	}


	int Interface::updateInterfaceFacet_bind(LuaState& lua) {
		Entity e = lua.pullStack<int>(1);

		bool draggable = lua["draggable"];
		bool hoverable = lua["hoverable"];
		bool clickable = lua["clickable"];
		LuaFunction onClick = LuaFunction{};
		if (clickable) {
			onClick = lua["onClick"];
		}

		LuaFunction onDrag = LuaFunction{};
		if (draggable) {
			onDrag = lua["onDrag"];
		}

		auto facet = single<Interface>().updateFacet(e, draggable, hoverable, clickable, onClick, onDrag);

		lua.pushStack(facet->id());

		return 1;
	}

	int Interface::getKeyStates_bind(LuaState& lua) {

		return 0;
	}

	int Interface::getGamepadStates_bind(LuaState& lua) {

		return 0;
	}


	std::vector<Facet*> Interface::getFacets(Entity& e) {
		auto out = std::vector<Facet*>{};
		for (auto facet : _facets) {
			if (facet.of() == e) {
				out.push_back(&facet);
			}
		}
		return out;
	}

	void Interface::destroyFacets(Entity& e) {
		
		for (auto it = std::begin(_facets); it != std::end(_facets); ++it) {
			if (it->of() == e) {
				it = _facets.erase(it);
				--it;
			}
		}
	}

}