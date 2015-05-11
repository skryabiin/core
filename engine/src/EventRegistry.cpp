#include "EventRegistry.hpp"
#include "EntityPositionQuery.hpp"
#include "DimensionChangeEvent.hpp"
#include "OffsetChangeEvent.hpp"
#include "OrientationChangeEvent.hpp"
#include "AnimationChangeEvent.hpp"
#include "ColorChangeEvent.hpp"
#include "ColorModulationEvent.hpp"
#include "ScaleChangeEvent.hpp"
#include "TextContentChangeEvent.hpp"
#include "TextureChangeEvent.hpp"
#include "GamepadEvent.hpp"
#include "KeyboardEvent.hpp"
#include "MouseButtonEvent.hpp"
#include "MouseEvent.hpp"
#include "MouseMoveEvent.hpp"
#include "SDLEvent.hpp"
#include "AccelerationChangeEvent.hpp"
#include "CollisionState.hpp"
#include "ObjectInteractionEvent.hpp"
#include "PositionChangeEvent.hpp"
#include "VelocityChangeEvent.hpp"
#include "EntitiesInRectQuery.hpp"
#include "EntityLayerQuery.hpp"
#include "EntityPositionQuery.hpp"
#include "FacetDimensionQuery.hpp"
#include "ObjectCollisionQuery.hpp"
#include "DebugEvent.hpp"
#include "FacetPauseEvent.hpp"
#include "PlaySoundEvent.hpp"
#include "VolumeChangeEvent.hpp"

namespace core {

	std::map<std::string, std::function<bool(LuaState&)>>& EventRegistry::registeredLuaHandlers() {

		static std::map<std::string, std::function<bool(LuaState&)>> _registeredLuaHandlers;
		return _registeredLuaHandlers;
	}

	EventRegistration<DimensionChangeEvent> entityPositionQueryReg{};
	EventRegistration<OffsetChangeEvent> OffsetChangeEventReg{};
	EventRegistration<OrientationChangeEvent> OrientationChangeEventReg{};
	EventRegistration<AnimationChangeEvent> AnimationChangeEventReg{};
	EventRegistration<ColorChangeEvent> ColorChangeEventReg{};
	EventRegistration<ColorModulationEvent> ColorModulationEventReg{};
	EventRegistration<ScaleChangeEvent> ScaleChangeEventReg{};
	EventRegistration<TextContentChangeEvent> TextContentChangeEventReg{};
	EventRegistration<TextureChangeEvent> TextureChangeEventReg{};
	//EventRegistration<GamepadEvent> GamepadEventReg{};
	//EventRegistration<KeyboardEvent> KeyboardEventReg{};
	//EventRegistration<MouseButtonEvent> MouseButtonEventReg{};
	//EventRegistration<MouseEvent> MouseEventReg{};
	//EventRegistration<MouseMoveEvent> MouseMoveEventReg{};
	//EventRegistration<WrappedSdlEvent> SDLEventReg{};
	EventRegistration<AccelerationChangeEvent> AccelerationChangeEventReg{};
	//EventRegistration<CollisionState> CollisionStateReg{};
	//EventRegistration<ObjectInteractionEvent> ObjectInteractionEventReg{};
	EventRegistration<PositionChangeEvent> PositionChangeEventReg{};
	EventRegistration<VelocityChangeEvent> VelocityChangeEventReg{};
	EventRegistration<EntitiesInRectQuery> EntitiesInRectQueryReg{};
	EventRegistration<EntityLayerQuery> EntityLayerQueryReg{};
	EventRegistration<EntityPositionQuery> EntityPositionQueryReg{};
	EventRegistration<FacetDimensionQuery> FacetDimensionQueryReg{};
	//EventRegistration<ObjectCollisionQuery> ObjectCollisionQueryReg{};
	//EventRegistration<DebugEvent> DebugEventReg{};
	EventRegistration<FacetPauseEvent> FacetPauseEventReg{};
	EventRegistration<PlaySoundEvent> PlaySoundEventReg{};
	EventRegistration<VolumeChangeEvent> VolumeChangeEventReg{};
} //end namespace core