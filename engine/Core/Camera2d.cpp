
#include "lua.hpp"
#include "Camera2d.hpp"
#include "Math.hpp"
#include "Entity.hpp"
#include "Core.hpp"

namespace core {

	InitStatus Camera2d::initializeImpl() {
		auto& lua = single<Core>().lua();
		_worldScale = Vec2{ lua("Config")["scale"][1], lua("Config")["scale"][2] };
	
		_windowWidth = lua("Config")["window"][1];
		_windowHeight = lua("Config")["window"][2];
		_worldCenterPosition.x = _windowWidth / 2;
		_worldCenterPosition.y = _windowHeight / 2;
		return InitStatus::INIT_TRUE;
	}

	InitStatus Camera2d::resetImpl() {
		return InitStatus::INIT_FALSE;

	}
	void Camera2d::setWorldCenterPosition(Point p) {

		_worldCenterPosition = p;
	}

	Point Camera2d::worldCenterPosition() const {
		return _worldCenterPosition;
	}

	void Camera2d::setWorldScale(Vec2 scale) {
		_worldScale = scale;
	}

	Vec2 Camera2d::worldScale() const {
		return _worldScale;
	}

	/*
	bool Camera::positionDrawable(Drawable& drawable, SimpleRect<int>& rectified) {
	
		
		int x1 = (_windowWidth / 2) - roundFloat(_worldScale.x * (_worldCenterPosition.x - (drawable.targetRect.x1 * drawable.scale.x)));
		int y1 = (_windowHeight / 2) + roundFloat(_worldScale.y * (_worldCenterPosition.y - (drawable.targetRect.y1 * drawable.scale.x)));

		int x2 = x1 + roundFloat(_worldScale.x * (drawable.targetRect.width() * drawable.scale.x));
		int y2 = y1 - roundFloat(_worldScale.y * (drawable.targetRect.height() * drawable.scale.y));

		if (x1 > x2) std::swap(x1, x2);
		if (y1 > y2) std::swap(y1, y2);
		rectified.x1 = x1;
		rectified.y1 = y1;
		rectified.x2 = x2;
		rectified.y2 = y2;		

		bool inWindow = (x2 > 0 || y2 > 0 || x1 < _windowWidth || y1 < _windowHeight);
		return inWindow;

	}
	*/

	void Camera2d::positionRect(SDL_Rect& rect) {
		rect.x = (_windowWidth / 2) - (_worldCenterPosition.x - rect.x);
		rect.y = (_windowHeight / 2) - (_worldCenterPosition.y - rect.y);
	}

	void Camera2d::positionRect(SimpleRect<int>& rect, Vec2& scale) {
		int x1 = (_windowWidth / 2) - roundFloat(_worldScale.x * (_worldCenterPosition.x - (rect.x1 * scale.x)));
		int y1 = (_windowHeight / 2) - roundFloat(_worldScale.y * (_worldCenterPosition.y - (rect.y1 * scale.y)));

		int x2 = x1 + roundFloat(_worldScale.x * (rect.width() * scale.x));
		int y2 = y1 - roundFloat(_worldScale.y * (rect.height() * scale.y));
		if (x1 > x2) std::swap(x1, x2);
		if (y1 > y2) std::swap(y1, y2);
		rect.x1 = x1;
		rect.y1 = y1;
		rect.x2 = x2;
		rect.y2 = y2;

	}


} //namespace core