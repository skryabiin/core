#include "RenderableSystem2d.hpp"
#include "PhysicsFacet.hpp"
#include "Core.hpp"

namespace core {



	InitStatus RenderableSystem2d::initializeImpl() {

		this->System::initializeImpl();

		std::function<void(RenderableSystem2d*, PositionChangeEvent&)> positionChangeCallback = std::mem_fn(&RenderableSystem2d::updateDrawPosition);
		_moveFilter.init(this, positionChangeCallback);
		single<EventProcessor>().addFilter(&_moveFilter);

		std::function<void(RenderableSystem2d*, EntityLayerQuery&)> entityLayerQueryHandler = std::mem_fn(&RenderableSystem2d::handleEntityLayerQuery);
		_entityLayerQueryFilter.init(this, entityLayerQueryHandler);
		single<EventProcessor>().addFilter(&_entityLayerQueryFilter);

		std::function<void(RenderableSystem2d*, ColorChangeEvent&)> colorChangeHandler = std::mem_fn(&RenderableSystem2d::updateColor);
		_colorChangeFilter.init(this, colorChangeHandler);
		single<EventProcessor>().addFilter(&_colorChangeFilter);

		return _camera.initialize();
	}

	InitStatus RenderableSystem2d::resetImpl() {
		_camera.reset();
		single<EventProcessor>().removeFilter(&_colorChangeFilter);
		single<EventProcessor>().removeFilter(&_entityLayerQueryFilter);
		single<EventProcessor>().removeFilter(&_moveFilter);
		return System::resetImpl();
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

	void RenderableSystem2d::handleFacetPauseEvent(FacetPauseEvent& pauseEvent) {		
		if (_cameraFollow.of() == pauseEvent.entity) {
			if (pauseEvent.facetId == -1 || _cameraFollow.id() == pauseEvent.facetId) {
				if (pauseEvent.paused) {
					_cameraFollow.pause();
				}
				else {
					_cameraFollow.resume();
				}
			}
		}
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



	EventProcessor::EventRegistration<EntityLayerQuery> entityLayerQueryReg{};


} //end namespace core