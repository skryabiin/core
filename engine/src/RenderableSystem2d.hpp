#ifndef CORE_RENDERABLE_SYSTEM_2D_HPP
#define CORE_RENDERABLE_SYSTEM_2D_HPP

#include "System.hpp"
#include "Templates.hpp"
#include "Camera2d.hpp"
#include "CameraFollowFacet.hpp"
#include "PositionChangeEvent.hpp"
#include "ColorChangeEvent.hpp"
#include "EntityLayerQuery.hpp"
#include "OffsetChangeEvent.hpp"
#include "ColorModulationEvent.hpp"
#include  "ScaleChangeEvent.hpp"
#include "SDL.h"


namespace core {


	class RenderableSystem2d : public virtual System, public EventListener<ScaleChangeEvent>, public EventListener<PositionChangeEvent>, public EventListener<EntityLayerQuery>,
		public EventListener<OffsetChangeEvent>, public EventListener<ColorModulationEvent> {

	public:

		RenderableSystem2d() {};

		virtual bool createImpl() override;
		virtual bool initializeImpl() override;
		virtual bool resetImpl() override;
		virtual bool destroyImpl() override;

		void setDrawableLayerId(int drawableLayerId);

		int RenderableSystem2d::getDrawableLayerId();


		CameraFollowFacet& createCameraFollowFacet(Entity& e);


		bool handleEvent(FacetPauseEvent& pauseEvent);

		bool handleEvent(ScaleChangeEvent& scaleChange);		

		virtual bool handleEvent(PositionChangeEvent& positionChange);

		bool handleEvent(EntityLayerQuery& entityLayerQuery);

		bool handleEvent(OffsetChangeEvent& offsetChangeEvent);

		bool handleEvent(ColorModulationEvent& colorModulationEvent);

		//this is expensive
		void snapCameraToEntity();

		void snapCameraToCoordinates(float x, float y);

		Camera2d* getCamera();

		static int createFacet_bind(LuaState& lua);

		virtual void renderFacet(SDL_Renderer* renderer, Facet* facet) {};

		virtual ~RenderableSystem2d() {}

	protected:

		int _drawableLayerId;

		CameraFollowFacet _cameraFollow;
		Camera2d _camera;


	};



} //end namespace core

#endif