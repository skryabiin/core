#ifndef CORE_RENDERABLE_SYSTEM_2D_HPP
#define CORE_RENDERABLE_SYSTEM_2D_HPP

#include "System.hpp"
#include "Templates.hpp"
#include "Camera2d.hpp"
#include "CameraFollowFacet.hpp"
#include "PositionChangeEvent.hpp"
#include "ColorChangeEvent.hpp"
#include "EntityLayerQuery.hpp"
#include "SDL.h"


namespace core {


	class RenderableSystem2d : public virtual System {

	public:

		RenderableSystem2d() {};

		virtual InitStatus initializeImpl();

		virtual InitStatus resetImpl();		

		void setDrawableLayerId(int drawableLayerId);

		int RenderableSystem2d::getDrawableLayerId();

		virtual void handleFacetPauseEvent(FacetPauseEvent& pauseEvent) override;

		CameraFollowFacet& createCameraFollowFacet(Entity& e);

		virtual void updateDrawPosition(PositionChangeEvent& positionChange) {};

		virtual void updateColor(ColorChangeEvent& colorChangeEvent) {};

		virtual void handleEntityLayerQuery(EntityLayerQuery& entityLayerQuery) {};

		//this is expensive
		void snapCameraToEntity();

		void snapCameraToCoordinates(float x, float y);

		Camera2d* getCamera();

		static int createFacet_bind(LuaState& lua);

		virtual void renderFacet(SDL_Renderer* renderer, Facet* facet) {};

	protected:

		int _drawableLayerId;

		CameraFollowFacet _cameraFollow;
		Camera2d _camera;

		EventFilter<PositionChangeEvent> _moveFilter;
		EventFilter<EntityLayerQuery> _entityLayerQueryFilter;
		EventFilter<ColorChangeEvent> _colorChangeFilter;


	};



} //end namespace core

#endif