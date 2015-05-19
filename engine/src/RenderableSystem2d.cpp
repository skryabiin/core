#include "RenderableSystem2d.hpp"
#include "PhysicsFacet.hpp"
#include "Core.hpp"
#include "VisualFacet.hpp"
#include "World.hpp"

namespace core {

	bool RenderableSystem2d::createImpl() {
		if (!this->System::createImpl()) {
			return false;
		}
		//return (_camera.create() == InitStatus::CREATE_TRUE) ? true : false;
		_camera = single<World>().camera();
		return true;
	}

	bool RenderableSystem2d::initializeImpl() {

		this->System::initializeImpl();

		single<Renderer>().createRenderLayer(_drawableLayerId, _camera);
		//return (_camera.initialize() == InitStatus::INIT_TRUE) ? true : false;
		return true;
	}

	bool RenderableSystem2d::resetImpl() {
		//_camera.reset();
		return System::resetImpl();
	}

	bool RenderableSystem2d::destroyImpl() {
		//_camera.destroy();
		return System::destroyImpl();
	}

	void RenderableSystem2d::snapCameraToCoordinates(float x, float y) {
		//_camera.setWorldCenterPosition(Point{ x, y });

	}

	void RenderableSystem2d::snapCameraToEntity() {
		auto physics = single<Core>().getFacetsByType<PhysicsFacet>(_cameraFollow.of());
		if (!physics.empty())  {

			auto& pf = physics[0];
			auto x = pf->p.x + 0.5f * pf->dim.w;
			auto y = pf->p.y + 0.5f * pf->dim.h;

			//_camera.setWorldCenterPosition(Point{ x, y });
		}
	}

	void RenderableSystem2d::setCamera(Camera* camera) {
		_camera = camera;
	}

	void RenderableSystem2d::setDrawableLayerId(int drawableLayerId) {
		_drawableLayerId = drawableLayerId;
	}

	int RenderableSystem2d::getDrawableLayerId() {
		return _drawableLayerId;
	}

	Camera* RenderableSystem2d::camera() {
		return _camera;
	}

	CameraFollowFacet& RenderableSystem2d::createCameraFollowFacet(Entity& e) {
		_cameraFollow.setOf(e);
		snapCameraToEntity();
		return _cameraFollow;
	}

	bool RenderableSystem2d::handleEvent(ScaleChangeEvent& scaleChange) {
		auto facets = getFacets(scaleChange.entity);
		for (auto& facet : facets) {
			if (facet->id() == scaleChange.facetId || scaleChange.facetId == -1) {
				auto newScale = scaleChange.scale.getVec2();
				auto vfacet = static_cast<VisualFacet*>(facet);
				vfacet->scale = scaleChange.scale.getVec2();
				updateDrawablePosition(vfacet);
				if (scaleChange.facetId != -1) return false;
			}
		}
		return true;
	}

	bool RenderableSystem2d::handleEvent(EntityLayerQuery& entityLayerQuery) {
		auto facets = getFacets(entityLayerQuery.entity);
		for (auto& facet : facets) {
			if (_drawableLayerId > entityLayerQuery.layerId) {
				entityLayerQuery.layerId = _drawableLayerId;
				entityLayerQuery.found = true;				
			}		
		}
		return (!entityLayerQuery.found);
	}

	bool RenderableSystem2d::handleEvent(FacetPauseEvent& pauseEvent) {

		if (_cameraFollow.of() == pauseEvent.entity && pauseEvent.facetId == -1) {
			if (pauseEvent.paused) {
				_cameraFollow.pause();
			}
			else {
				_cameraFollow.resume();
			}
		}

		auto facets = getFacets(pauseEvent.entity);
		for (auto& facet : facets) {			

			if (pauseEvent.facetId == -1 || facet->id() == pauseEvent.facetId) {
				auto vfacet = static_cast<VisualFacet*>(facet);
				auto dc = DrawableChange{};
				dc.operation = DrawableChange::Operation::PAUSE;
				dc.facetId = facet->id();
				dc.layerId = _drawableLayerId;
				dc.paused = pauseEvent.paused;
				if (pauseEvent.paused) {					
					vfacet->pause();					
				}
				else {
					vfacet->resume();					
				}
				single<Renderer>().applyDrawableChange(dc);
				if (facet->id() == pauseEvent.facetId) return false;
			}
		}
		return true;
	}

	bool RenderableSystem2d::handleEvent(OffsetChangeEvent& offsetChangeEvent) {
		auto facets = getFacets(offsetChangeEvent.entity);
		for (auto& facet : facets) {
			if (offsetChangeEvent.facetId == -1 || offsetChangeEvent.facetId == facet->id()) {
				
				auto vfacet = static_cast<VisualFacet*>(facet);
				vfacet->offset = offsetChangeEvent.offset.getPixel();
				updateDrawablePosition(vfacet);

				if (offsetChangeEvent.facetId != -1) return false;
			}
		}
		return true;
	}

	bool RenderableSystem2d::handleEvent(PositionChangeEvent& positionChange) {


		auto facets = getFacets(positionChange.entity);
		for (auto& facet : facets) {			
			auto vfacet = static_cast<VisualFacet*>(facet);
			auto p = positionChange.position.getPixel();
			if (positionChange.relative) {
				vfacet->position += p;				
			}
			else {
				vfacet->position = p;
			}
			if (_cameraFollow.of() == positionChange.entity && !_cameraFollow.isPaused()) {
				auto x = vfacet->position.x + vfacet->offset.x + vfacet->dimensions.w * vfacet->scale.x * 0.5f;
				auto y = vfacet->position.y + vfacet->offset.y + vfacet->dimensions.h * vfacet->scale.y * 0.5f;
				snapCameraToCoordinates(x, y);
			}

			updateDrawablePosition(vfacet);				
		}
		return true;
	}

	

	bool RenderableSystem2d::handleEvent(ColorModulationEvent& colorModulationEvent) {

		auto facets = getFacets(colorModulationEvent.entity);
		for (auto facet : facets) {
			if (colorModulationEvent.facetId == -1 || colorModulationEvent.facetId == facet->id()) {
				auto& valList = colorModulationEvent.matrix;
				auto dc = DrawableChange{};
				dc.facetId = facet->id();
				dc.layerId = _drawableLayerId;
				dc.operation = DrawableChange::Operation::CHANGE_COLOR_TRANSFORM;
				dc.colorTransform = colorModulationEvent.transform;
				single<Renderer>().applyDrawableChange(dc);
				if (colorModulationEvent.facetId != -1) return false;
			}
		}
		return true;
	}

	int RenderableSystem2d::createFacet_bind(LuaState& lua) {

		auto systemName = lua.pullStack<std::string>(1);
		auto entity = lua.pullStack<int>(2);

		auto system = single<Core>().getSystemByName<RenderableSystem2d>(systemName);

		if (system != nullptr) {
			auto& newFacet = system->createCameraFollowFacet(entity);
			lua.pushStack(newFacet.id());
		}
		else {
			lua.pushStack(-1L);
		}
		return 0;
	}


} //end namespace core