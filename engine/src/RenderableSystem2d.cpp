#include "RenderableSystem2d.hpp"
#include "PhysicsFacet.hpp"
#include "Core.hpp"
#include "VisualFacet.hpp"

namespace core {

	bool RenderableSystem2d::createImpl() {
		if (!this->System::createImpl()) {
			return false;
		}
		return (_camera.create() == InitStatus::CREATE_TRUE) ? true : false;
	}

	bool RenderableSystem2d::initializeImpl() {

		this->System::initializeImpl();

		return (_camera.initialize() == InitStatus::INIT_TRUE) ? true : false;
	}

	bool RenderableSystem2d::resetImpl() {
		//_camera.reset();
		return System::resetImpl();
	}

	bool RenderableSystem2d::destroyImpl() {
		_camera.destroy();
		return System::destroyImpl();
	}

	void RenderableSystem2d::snapCameraToCoordinates(float x, float y) {
		_camera.setWorldCenterPosition(Point{ x, y });

	}

	void RenderableSystem2d::snapCameraToEntity() {
		auto physics = single<Core>().getFacetsByType<PhysicsFacet>(_cameraFollow.of());
		if (!physics.empty())  {

			auto& pf = physics[0];
			auto x = pf->p.x + 0.5f * pf->dim.w;
			auto y = pf->p.y + 0.5f * pf->dim.h;

			_camera.setWorldCenterPosition(Point{ x, y });
		}
	}



	void RenderableSystem2d::setDrawableLayerId(int drawableLayerId) {
		_drawableLayerId = drawableLayerId;
	}

	int RenderableSystem2d::getDrawableLayerId() {
		return _drawableLayerId;
	}

	Camera2d* RenderableSystem2d::getCamera() {
		return &_camera;
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
				auto oldScale = vfacet->scale;
				auto rect = vfacet->drawable.targetRect;
				rect.w = roundFloat((rect.w / oldScale.x) * newScale.x);
				rect.h = roundFloat((rect.h / oldScale.y) * newScale.y);
				vfacet->drawable.targetRect = rect;
				vfacet->scale = newScale;
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
				if (pauseEvent.paused) {					
					vfacet->pause();
					single<Renderer>().pauseDrawable(vfacet->drawable);
				}
				else {
					vfacet->resume();
					single<Renderer>().resumeDrawable(vfacet->drawable);
				}
				if (facet->id() == pauseEvent.facetId) return false;
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
				vfacet->drawable.targetRect.x += p.x;
				vfacet->drawable.targetRect.y += p.y;
				vfacet->drawable.zIndex += p.z;
			}
			else {
				vfacet->drawable.targetRect.x = p.x + vfacet->offset.x;
				vfacet->drawable.targetRect.y = p.y + vfacet->offset.y;
				vfacet->drawable.zIndex = p.z + vfacet->offset.z;
			}
			if (_cameraFollow.of() == positionChange.entity && !_cameraFollow.isPaused()) {
				auto x = vfacet->drawable.targetRect.x + vfacet->drawable.targetRect.w * 0.5f;
				auto y = vfacet->drawable.targetRect.y + vfacet->drawable.targetRect.h * 0.5f;
				snapCameraToCoordinates(x, y);
			}

			single<Renderer>().updateDrawable(vfacet->drawable);
			return true;
	
		}
		return true;
	}

	bool RenderableSystem2d::handleEvent(OffsetChangeEvent& offsetChangeEvent) {
		auto facets = getFacets(offsetChangeEvent.entity);
		for (auto& facet : facets) {
			if (offsetChangeEvent.facetId == -1 || offsetChangeEvent.facetId == facet->id()) {
				auto offset = offsetChangeEvent.offset.getPixel();
				auto vfacet = static_cast<VisualFacet*>(facet);
				auto currentOffset = vfacet->offset;
				auto rect = vfacet->drawable.targetRect;
				rect.x = rect.x - currentOffset.x + offset.x;
				rect.y = rect.y - currentOffset.y + offset.y;
				vfacet->drawable.targetRect = rect;
				vfacet->drawable.zIndex = vfacet->drawable.zIndex - currentOffset.z + offset.z;
				vfacet->offset = offset;
				single<Renderer>().updateDrawable(vfacet->drawable);
				if (offsetChangeEvent.facetId != -1) return false;
			}
		}
		return true;
	}

	bool RenderableSystem2d::handleEvent(ColorModulationEvent& colorModulationEvent) {

		auto facets = getFacets(colorModulationEvent.entity);
		for (auto facet : facets) {
			if (colorModulationEvent.facetId == -1 || colorModulationEvent.facetId == facet->id()) {
				auto& valList = colorModulationEvent.matrix;
				auto& d = (static_cast<VisualFacet*>(facet))->drawable;
				for (int i = 0; i < 4; ++i) {
					d.colorTransform.setChannel(i, valList[i * 4], valList[i * 4 + 1], valList[i * 4 + 2], valList[i * 4 + 3]);
				}
				single<Renderer>().updateDrawable(d);
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