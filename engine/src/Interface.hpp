#ifndef CORE_INTERFACE_HPP
#define CORE_INTERFACE_HPP

#include "Templates.hpp"
#include "Geometry.hpp"
#include "MouseEvent.hpp"
#include "TextureRenderSystem2d.hpp"
#include "InterfaceFacet.hpp"
#include "UpdateableSystem.hpp"
#include "PositionChangeEvent.hpp"
#include "DimensionChangeEvent.hpp"
#include "PrimitiveRenderSystem2d.hpp"

namespace core {

	struct MouseCursorDef {

		MouseCursorDef() : isSet{ false } {}

		std::string textureName; 
		SDL_Rect sourceRect; 
		Color textureColorMod;
		Dimension dimensions; 
		Vec2 scale; 
		Pixel offset;
		bool isSet;
	};

	struct InterfaceState : public LuaTable {

		InterfaceState() : pickedUpThisTick{ false }, selectedLayerId{ -1 } {
			pickedUp = nullptr;
			lua_reg("pickedUpThisTick", &pickedUpThisTick);
			lua_reg("selectedLayerId", &selectedLayerId);
			lua_reg("clickPosition", &clickPosition);
			lua_reg("currentPosition", &currentPosition);
			lua_reg("pickedUpPosition", &pickedUpPosition);
			lua_reg("rawClickPosition", &rawClickPosition);
			lua_reg("rawCurrentPosition", &rawCurrentPosition);
			lua_reg("rawPickedUpPosition", &rawPickedUpPosition);
		}
		
		InterfaceFacet* pickedUp;		
		LuaPixel clickPosition;
		LuaPixel currentPosition;
		LuaPixel pickedUpPosition;
		LuaPixel rawClickPosition;
		LuaPixel rawCurrentPosition;
		LuaPixel rawPickedUpPosition;
		InterfaceFacet* hovering;
		InterfaceFacet* clicked;
		bool pickedUpThisTick;
		int selectedLayerId;

	};

	class Interface : public UpdateableSystem, public singleton<Interface>, public EventListener<PositionChangeEvent>, public EventListener<DimensionChangeEvent> {

	public:


		Interface();

		virtual bool createImpl() override;
		virtual bool initializeImpl() override;
		virtual bool resetImpl() override;
		virtual bool destroyImpl() override;

		void updateImpl(float dt, RuntimeContext& runtimeContext);

		virtual std::vector<Facet*> getFacets(Entity& e) override;

		void updateMouseState();

		Pixel getMouseCursorPos();

		void setCursorTexture(std::string textureName, SDL_Rect sourceRect, Color textureColorMod, Dimension dimensions, Vec2 scale, Pixel offset);

		MouseEvent getMouseState();		

		void pollSdlEvents();

		static int getMouseState_bind(LuaState& lua);		

		virtual void destroyFacets(Entity& e) override;

		bool handleEvent(FacetPauseEvent& pauseEvent);

		bool handleEvent(PositionChangeEvent& positionChangeEvent);
		
		bool handleEvent(DimensionChangeEvent& dimensionChangeEvent);

		void checkGamepadStatus();

		void showHideTextureCursor(bool show);

		void showHideSystemCursor(bool show);

		void useSystemCursor();

		void useTextureCursor();

		void showCursor();

		void hideCursor();

		static int setCursorTexture_bind(LuaState& lua);		

		static int createInterfaceFacet_bind(LuaState& lua);

		static int updateInterfaceFacet_bind(LuaState& lua);

		static int getKeyStates_bind(LuaState& lua);

		static int getGamepadStates_bind(LuaState& lua);

		static int showCursor_bind(LuaState& lua);

		static int hideCursor_bind(LuaState& lua);

		static int useSystemCursor_bind(LuaState& lua); 
		
		static int useTextureCursor_bind(LuaState& lua);

		void updateFacet(int facetId, Camera* cameraContext, bool draggable, bool hoverable, bool clickable, LuaFunction& onClick, LuaFunction& offClick, LuaFunction& onHover, LuaFunction& offHover, LuaFunction& onDrag);

		InterfaceFacet* createFacet(Entity& e, Pixel& position, Dimension& dimensions, Camera* cameraContext, bool draggable, bool hoverable, bool clickable, LuaFunction& onClick, LuaFunction& offClick, LuaFunction& onHover, LuaFunction& offHover, LuaFunction& onDrag);		

		Camera* camera();

	private:


		void setCursorTextureFromDef();

		MouseCursorDef _mouseCursorDef;

		TextureRenderSystem2d* _textureRenderSystem;

		PrimitiveRenderSystem2d* _primitiveRenderSystem;

		InterfaceState _interfaceState;

		MouseEvent _mouseState;

		SDL_GameController* _sdlGamepad;

		Entity _mouseCursor;

		std::vector<InterfaceFacet> _facets;

		Camera _camera;

		bool _usingSystemCursor;

	};

} //end namespace core

#endif