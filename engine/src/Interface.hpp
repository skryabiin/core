#ifndef CORE_INTERFACE_HPP
#define CORE_INTERFACE_HPP

#include "Templates.hpp"
#include "Geometry.hpp"
#include "MouseEvent.hpp"
#include "TextureRenderSystem2d.hpp"
#include "InterfaceFacet.hpp"
#include "UpdateableSystem.hpp"


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
			lua_reg("clickPosition", &clickPosition);
			lua_reg("currentPosition", &currentPosition);
			lua_reg("pickedUpThisTick", &pickedUpThisTick);
			lua_reg("selectedLayerId", &selectedLayerId);
			lua_reg("pickedUpPosition", &pickedUpPosition);
		}
		
		InterfaceFacet* pickedUp;		
		LuaPixel clickPosition;
		LuaPixel currentPosition;
		LuaPixel pickedUpPosition;
		bool pickedUpThisTick;
		int selectedLayerId;

	};

	class Interface : public UpdateableSystem, public singleton<Interface>{

	public:


		Interface();

		virtual bool createImpl() override;
		virtual bool initializeImpl() override;
		virtual bool resetImpl() override;
		virtual bool destroyImpl() override;

		void updateImpl(RuntimeContext& runtimeContext);

		virtual std::vector<Facet*> getFacets(Entity& e) override;

		void updateMouseState();

		Pixel getMouseCursorPos();

		void setCursorTexture(std::string textureName, SDL_Rect sourceRect, Color textureColorMod, Dimension dimensions, Vec2 scale, Pixel offset);

		MouseEvent getMouseState();		

		void pollSdlEvents();

		static int getMouseState_bind(LuaState& lua);		

		virtual void destroyFacets(Entity& e) override;

		bool handleEvent(FacetPauseEvent& pauseEvent);

		void checkGamepadStatus();

		static int setCursorTexture_bind(LuaState& lua);		

		static int updateInterfaceFacet_bind(LuaState& lua);

		static int getKeyStates_bind(LuaState& lua);

		static int getGamepadStates_bind(LuaState& lua);
		
		InterfaceFacet* updateFacet(Entity& e, bool draggable, bool hoverable, bool clickable, LuaFunction onClick, LuaFunction onDrag);

	private:


		void setCursorTextureFromDef();

		MouseCursorDef _mouseCursorDef;

		TextureRenderSystem2d* _textureRenderSystem;

		InterfaceState _interfaceState;

		MouseEvent _mouseState;

		SDL_GameController* _sdlGamepad;

		Entity _mouseCursor;

		std::vector<InterfaceFacet> _facets;


	};

} //end namespace core

#endif