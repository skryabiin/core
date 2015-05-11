
#include "lua.hpp"
#include "Camera2d.hpp"
#include "Math.hpp"
#include "Entity.hpp"
#include "Core.hpp"

namespace core {

	bool Camera2d::createImpl() {

		return true;
	}

	bool Camera2d::initializeImpl() {
		auto& lua = single<Core>().lua();
		_worldScale = Vec2{ lua("Config")["camera"]["scale"][1], lua("Config")["camera"]["scale"][2] };

		_windowWidth = lua("Config")["window"]["dimensions"][1];
		_windowHeight = lua("Config")["window"]["dimensions"][2];
		_worldCenterPosition.x = lua("Config")["camera"]["centerWorldPoint"][1];
		_worldCenterPosition.y = lua("Config")["camera"]["centerWorldPoint"][2];
		_viewportRect.w = _windowWidth;
		_viewportRect.h = _windowHeight;
		return true;
	}

	bool Camera2d::resetImpl() {
		return true;
	}

	bool Camera2d::destroyImpl() {
		return true;
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


	bool Camera2d::positionPoints(std::vector<Point>& points, std::vector<GLfloat>& vertices) {
		bool onScreen = true;		
		vertices.clear();
		for (auto& point : points) {
			
			//TODO check on screen
			vertices.push_back((GLfloat(_windowWidth / 2.0) - GLfloat(_worldCenterPosition.x - point.x)) / GLfloat(_windowWidth / 2.0));
			vertices.push_back((GLfloat(_windowHeight / 2.0) - GLfloat(_worldCenterPosition.y - point.y)) / GLfloat(_windowHeight / 2.0));
		}
		return true;

	}

	bool Camera2d::positionRect(Rect& rect, std::vector<GLfloat>& vertices) {
		_bufferRect.x = (_windowWidth / 2) - (_worldCenterPosition.x - rect.x);
		_bufferRect.y = (_windowHeight / 2) - (_worldCenterPosition.y - rect.y);
		_bufferRect.w = rect.w;
		_bufferRect.h = rect.h;
		if (!_bufferRect.intersects(_viewportRect)) {
			return false;
		}

		auto x0 = (GLfloat(_bufferRect.x) - _viewportRect.w / 2.0f) / (_windowWidth / 2.0f);
		auto x1 = (GLfloat(_bufferRect.x + _bufferRect.w) - _viewportRect.w / 2.0f) / (_windowWidth / 2.0f);
		auto y0 = (GLfloat(_bufferRect.y) - _viewportRect.h / 2.0f) / (_windowHeight / 2.0f);
		auto y1 = (GLfloat(_bufferRect.y + _bufferRect.h) - _viewportRect.h / 2.0f) / (_windowHeight / 2.0f);
		vertices.clear();
		vertices.push_back(x0);
		vertices.push_back(y0);
		vertices.push_back(x1);
		vertices.push_back(y0);
		vertices.push_back(x1);
		vertices.push_back(y1);
		vertices.push_back(x0);
		vertices.push_back(y1);
		return true;
	}

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