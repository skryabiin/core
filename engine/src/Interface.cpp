#include "Interface.hpp"
#include "Core.hpp"
#include "TextureRenderSystem2d.hpp"
#include "RenderableSystem2d.hpp"
#include "EntitiesInRectQuery.hpp"
#include "InterfaceFacet.hpp"
#include "EntityPositionQuery.hpp"
#include "EntityLayerQuery.hpp"
#include "KeyboardEvent.hpp"
#include "GamepadEvent.hpp"
#include "World.hpp"

namespace core {

	Interface::Interface() {
		_sdlGamepad = nullptr;

	}

	bool Interface::createImpl() {
		info("Creating interface...");
		if (!this->System::createImpl()) return false;

		SDL_Init(SDL_INIT_GAMECONTROLLER);
		checkGamepadStatus();


		auto& lua = single<Core>().lua();
		lua.bindFunction("getMouseState_bind", Interface::getMouseState_bind);
		lua.bindFunction("setCursorTexture_bind", Interface::setCursorTexture_bind);
		lua.bindFunction("createInterfaceFacet_bind", Interface::createInterfaceFacet_bind);
		lua.bindFunction("updateInterfaceFacet_bind", Interface::updateInterfaceFacet_bind);
		lua.bindFunction("getKeyStates_bind", Interface::getKeyStates_bind);
		lua.bindFunction("getGamepadStates_bind", Interface::getGamepadStates_bind);
		lua.bindFunction("showCursor_bind", Interface::showCursor_bind);			
		lua.bindFunction("hideCursor_bind", Interface::hideCursor_bind);
		lua.bindFunction("useSystemCursor_bind", Interface::useSystemCursor_bind);
		lua.bindFunction("useTextureCursor_bind", Interface::useTextureCursor_bind);
		
		_usingSystemCursor = true;

		_camera.create();
		return true;

	}

	bool Interface::initializeImpl() {

		this->System::initializeImpl();


		_camera.initialize();

		_textureRenderSystem = new TextureRenderSystem2d{};
		_textureRenderSystem->setName("InterfaceTextures");
		single<Core>().addSystem(_textureRenderSystem);
		single<Core>().includeRenderableSystem2d(_textureRenderSystem);
		_textureRenderSystem->create();
		_textureRenderSystem->initialize();
		_textureRenderSystem->setDrawableLayerId(0);
		_textureRenderSystem->setCamera(&_camera);
		_mouseCursor = single<Core>().createEntity();
		setCursorTextureFromDef();

		_primitiveRenderSystem = new PrimitiveRenderSystem2d{};
		_primitiveRenderSystem->setName("InterfacePrimitives");
		single<Core>().addSystem(_primitiveRenderSystem);
		single<Core>().includeRenderableSystem2d(_primitiveRenderSystem);
		_primitiveRenderSystem->create();
		_primitiveRenderSystem->initialize();
		_primitiveRenderSystem->setDrawableLayerId(0);
		auto offset = Pixel{ 0, 0, 1 };
		auto rect = SDL_Rect();
		rect.x = 10;
		rect.y = 20;
		rect.w = 20;
		rect.h = 40;
		auto color = Color{ 0.0f, 1.0f, 1.0f, 1.0f };
		auto position = Pixel{10,20};		
		auto dimension = Dimension{ 20, 40 };
		//_primitiveRenderSystem->createRectangleFacet(_mouseCursor, position, offset, dimension, color, false);
		
		return true;
	}

	bool Interface::resetImpl() {
		
		_facets.clear();
		_facets.shrink_to_fit();
		_textureRenderSystem = nullptr;
		_primitiveRenderSystem = nullptr;

		_camera.reset();
		return this->System::resetImpl();		
	}

	bool Interface::destroyImpl() {
		_camera.destroy();
		return this->System::destroyImpl();
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
				else if (e.type == SDL_QUIT) {
					WrappedSdlEvent sdlEvent{};
					sdlEvent._wrappedEvent = &e;
					single<EventProcessor>().process(sdlEvent);
				}
				else if (e.type != SDL_MOUSEBUTTONDOWN && e.type != SDL_MOUSEBUTTONUP && e.type != SDL_MOUSEMOTION) {
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



	void Interface::updateImpl(float dt, RuntimeContext& runtimeContext) {

		pollSdlEvents();
		updateMouseState();

		auto p = _mouseState.position.getPixel();

	}

	Camera* Interface::camera() {
		return &_camera;
	}


	void Interface::updateMouseState() {

		auto pos = Pixel{};
		auto keyStates = SDL_GetMouseState(&pos.x, &pos.y);
		auto& lua = single<Core>().lua();
		int winHeight = lua("Config")["window"]["dimensions"][2];
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
			static_cast<RenderableSystem2d*>(_textureRenderSystem)->handleEvent(cursorPce);



			_interfaceState.rawCurrentPosition.setPixel(pos);
			_interfaceState.pickedUpThisTick = false;

			//if something was already picked up
			if (_interfaceState.pickedUp != nullptr) {

				//if the mouse is held down, don't do anything except drag
				if (_mouseState.mdown && _mouseState.mdownOld) {
					//if it's draggable, do so
					if (_interfaceState.pickedUp->draggable) {
						_interfaceState.currentPosition.setPixel(_interfaceState.pickedUp->camera->alignPoint(pos));						
						lua.call(_interfaceState.pickedUp->onDrag, _interfaceState);
						_interfaceState.currentPosition.setPixel(pos);
					}
					return;
				}

				//if the mouse was released, let it go and call 'offClick'
				if (!_mouseState.mdown && _mouseState.mdownOld && _interfaceState.pickedUp->clickable) {					
					_interfaceState.currentPosition.setPixel(_interfaceState.pickedUp->camera->alignPoint(pos));
					lua.call(_interfaceState.pickedUp->offClick, _interfaceState);
					_interfaceState.pickedUp = nullptr;
				}
			}			

			//these will be updated as the results are parsed
			auto selectedLayer = 10000;
			auto selectedPos = Pixel{ 0, 0, 10000 };
			InterfaceFacet* topFacetUnderCursor = nullptr;
			
			//check for any facets underneath the cursor
			for (auto& facet : _facets) {
				//if this entity has an interface facet that is not paused
				if (facet.isPaused()) continue;
				auto aligned = facet.camera->alignPoint(pos);
				if (_mouseState.mdown) {
					debug("Aligned cursor position: ", aligned.x, ", ", aligned.y);
				}
				//if the entity is under the cursor
				if (inRect(aligned.x, aligned.y, facet.position.x, facet.position.y, facet.position.x + facet.dimensions.w, facet.position.y + facet.dimensions.h)) {

					//we need to check the layer to see if this is on top
					auto entityLayerQuery = EntityLayerQuery{};
					entityLayerQuery.entity = facet.of();
					single<EventProcessor>().process(entityLayerQuery);

					if (entityLayerQuery.found) {
						//if the entity is in a closer than the previous selected layer
						if (entityLayerQuery.found && entityLayerQuery.layerId < selectedLayer) {
							selectedLayer = entityLayerQuery.layerId;
							selectedPos = facet.position;
							topFacetUnderCursor = &facet;
							
						}
						//else if it in the same layer, check the z positions
						else if (entityLayerQuery.layerId == selectedLayer) {
							if (facet.position.z <= selectedPos.z) {
								topFacetUnderCursor = &facet;
								selectedPos = facet.position;
							}
						}
					}
				}
			}

			//if there's an entity under the cursor
			if (topFacetUnderCursor != nullptr) {

				//if the mouse is clicked, pick up the entity
				if (_mouseState.mdown && !_mouseState.mdownOld && topFacetUnderCursor->clickable) {
					
					_interfaceState.clickPosition.setPixel(pos);
					_interfaceState.pickedUp = topFacetUnderCursor;
					_interfaceState.clickPosition.setPixel(_interfaceState.pickedUp->camera->alignPoint(pos));					
					_interfaceState.currentPosition.setPixel(_interfaceState.pickedUp->camera->alignPoint(pos));
					_interfaceState.pickedUpThisTick = true;
					_interfaceState.pickedUpPosition.setPixel(selectedPos);

					if (_interfaceState.hovering != nullptr) {							
						lua.call(_interfaceState.hovering->offHover, _interfaceState);
						_interfaceState.hovering = nullptr;
					}
					lua.call(_interfaceState.pickedUp->onClick, _interfaceState);

					if (_interfaceState.pickedUp->draggable) {
						lua.call(_interfaceState.pickedUp->onDrag, _interfaceState);
					}
				}

				//if the mouse isn't clicked, hover over the entity
				else if (!_mouseState.mdown && topFacetUnderCursor->hoverable) {

					bool newHover = false;
					if (_interfaceState.hovering == nullptr) {
						newHover = true;
					}
					else if (*_interfaceState.hovering != *topFacetUnderCursor) {
						lua.call(_interfaceState.hovering->offHover, _interfaceState);
						newHover = true;
					}

					if (newHover) {
						_interfaceState.hovering = topFacetUnderCursor;
						lua.call(_interfaceState.hovering->onHover, _interfaceState);
					}
				}
			}
			//if there are no entities under the cursor
			else {
				//if the cursor was previously hovering over an entity, stop hovering
				if (_interfaceState.hovering != nullptr) {
					lua.call(_interfaceState.hovering->offHover, _interfaceState);
					_interfaceState.hovering = nullptr;
				}
			}
		}
	}


	bool Interface::handleEvent(PositionChangeEvent& positionChangeEvent) {
		for (auto& facet : _facets) {
			if (facet.of() == positionChangeEvent.entity) {
				if (positionChangeEvent.relative) {
					facet.position = facet.position + positionChangeEvent.position.getPixel();
				}
				else {
					facet.position = positionChangeEvent.position.getPixel();
				}
				return true;
			}
		}
		return true;
	}

	bool Interface::handleEvent(DimensionChangeEvent& dimensionChangeEvent) {

		for (auto& facet : _facets) {
			if (facet.of() == dimensionChangeEvent.entity) {
				facet.dimensions = dimensionChangeEvent.dimensions.getDimension();
				return true;
			}
		}
		return true;
	}


	bool Interface::handleEvent(FacetPauseEvent& pauseEvent) {

		for (auto& facet : _facets) {
			if (pauseEvent.entity == facet.of()) {

				if (pauseEvent.facetId == -1 || pauseEvent.facetId == facet.id()) {
					if (pauseEvent.paused) {
						facet.pause();
					}
					else {
						facet.resume();
					}
					if (pauseEvent.facetId != -1) {
						return false;
					}
				}
			}
		}
		return true;
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
			_textureRenderSystem->createTextureFacet(_mouseCursor, _mouseState.position.getPixel(), _mouseCursorDef.offset, _mouseCursorDef.scale, _mouseCursorDef.sourceRect, _mouseCursorDef.textureName);
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

	InterfaceFacet* Interface::createFacet(Entity& e, Pixel& position, Dimension& dimensions, Camera* cameraContext, bool draggable, bool hoverable, bool clickable, LuaFunction& onClick, LuaFunction& offClick, LuaFunction& onHover, LuaFunction& offHover, LuaFunction& onDrag) {
		auto facet = InterfaceFacet{};
		facet.setOf(e);	
		facet.position = position;
		facet.dimensions = dimensions;
		facet.draggable = draggable;
		facet.hoverable = hoverable;
		facet.clickable = clickable;
		facet.onClick = onClick;
		facet.offClick = offClick;
		facet.onHover = onHover;
		facet.offHover = offHover;
		facet.onDrag = onDrag;
		facet.camera = cameraContext;
		_facets.push_back(facet);
		return &(_facets.back());
	}

	void Interface::updateFacet(int facetId, bool draggable, bool hoverable, bool clickable, LuaFunction& onClick, LuaFunction& offClick, LuaFunction& onHover, LuaFunction& offHover, LuaFunction& onDrag) {		

		InterfaceFacet* facetToUpdate = nullptr;
		for (auto& facet : _facets) {

			if (facet.id() == facetId) {
				facet.draggable = draggable;
				facet.hoverable = hoverable;
				facet.clickable = clickable;
				facet.onClick = onClick;
				facet.offClick = offClick;
				facet.onHover = onHover;
				facet.offHover = offHover;
				facet.onDrag = onDrag;
				break;
			}
		}		
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
		int facetId = lua.pullStack<int>(1);

		bool draggable = lua["draggable"];
		bool hoverable = lua["hoverable"];
		bool clickable = lua["clickable"];
		LuaFunction onClick = LuaFunction{};
		if (clickable) {
			onClick = lua["onClick"];
		}

		LuaFunction offClick = LuaFunction{};
		if (clickable) {
			offClick = lua["offClick"];
		}

		LuaFunction onHover = LuaFunction{};
		if (hoverable) {
			onHover = lua["onHover"];
		}

		LuaFunction offHover = LuaFunction{};
		if (hoverable) {
			offHover = lua["offHover"];
		}

		LuaFunction onDrag = LuaFunction{};
		if (draggable) {
			onDrag = lua["onDrag"];
		}

		single<Interface>().updateFacet(facetId, draggable, hoverable, clickable, onClick, offClick, onHover, offHover, onDrag);

		return 0;

	}

	int Interface::createInterfaceFacet_bind(LuaState& lua) {
		Entity e = lua.pullStack<int>(1);

		LuaPixel position = lua["position"];
		LuaDimension dimensions = lua["dimensions"];


		/*
		std::string cameraName = lua["camera"];


		Camera* cameraContext;
		if (!cameraName.compare("world")) {
			cameraContext = single<World>().camera();
		}
		else if (!cameraName.compare("interface")) {
			cameraContext = single<Interface>().camera();
		}
		else {
			auto renderableSystem = single<Core>().getSystemByName<RenderableSystem2d>(cameraName);
			cameraContext = renderableSystem->camera();
		} */


		bool draggable = lua["draggable"];
		bool hoverable = lua["hoverable"];
		bool clickable = lua["clickable"];
		LuaFunction onClick = lua["onClick"];	
		LuaFunction offClick = offClick = lua["offClick"];
		LuaFunction onHover = onHover = lua["onHover"];
		LuaFunction offHover = lua["offHover"];
		LuaFunction onDrag = onDrag = lua["onDrag"];



		auto facet = single<Interface>().createFacet(e, position.getPixel(), dimensions.getDimension(), single<World>().camera(), draggable, hoverable, clickable, onClick, offClick, onHover, offHover, onDrag);

		lua.pushStack(facet->id());

		return 1;
	}

	int Interface::getKeyStates_bind(LuaState& lua) {

		return 0;
	}

	int Interface::getGamepadStates_bind(LuaState& lua) {

		return 0;
	}


	void Interface::useSystemCursor() {
		_usingSystemCursor = true;
		showHideTextureCursor(false);
		showHideSystemCursor(true);		
	}

	void Interface::useTextureCursor() {
		_usingSystemCursor = false;
		showHideSystemCursor(false);
		showHideTextureCursor(true);
	}

	void Interface::showHideTextureCursor(bool show) {
		auto facetQuery = _textureRenderSystem->getFacets(_mouseCursor);
		if (!facetQuery.empty()) {
			if (show) {
				facetQuery[0]->resume();
			}
			else {
				facetQuery[0]->pause();
			}
		}
	}

	void Interface::showHideSystemCursor(bool show) {
		SDL_ShowCursor((show) ? 1 : 0);
	}

	void Interface::showCursor() {
		if (_usingSystemCursor) {
			showHideSystemCursor(true);
		}
		else {
			showHideTextureCursor(true);
		}
	}

	void Interface::hideCursor() {

		if (_usingSystemCursor) {
			showHideSystemCursor(false);
		}
		else {
			showHideTextureCursor(false);
		}
	}


	int Interface::showCursor_bind(LuaState& lua) {
		single<Interface>().showCursor();
		return 0;
	}

	int Interface::hideCursor_bind(LuaState& lua) {
		single<Interface>().hideCursor();
		return 0;
	}

	int Interface::useSystemCursor_bind(LuaState& lua) {
		single<Interface>().useSystemCursor();
		return 0;
	}

	int Interface::useTextureCursor_bind(LuaState& lua) {
		single<Interface>().useTextureCursor();
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