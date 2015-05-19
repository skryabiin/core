#include "Camera.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtx/transform.hpp"
#include "gtx/rotate_vector.hpp"
#include "Core.hpp"

namespace core {

	Camera::Camera() {

	}

	bool Camera::createImpl() {
		return true;
	}

	bool Camera::initializeImpl() {		

		_windowDimensions = single<Renderer>().windowDimensions();

		_view = glm::mat4();
		_zoom = Vec2{ 1.0f, 1.0f };
		_theta = 0;
		_phi = 0;
		_psi = 0;
		_position = glm::vec3(0.0f, 0.0f, 0.0f);

		_recalculateProjection();

		return true;

	}

	bool Camera::resetImpl() {
		return true;
	}

	bool Camera::destroyImpl() {
		return true;
	}

	void Camera::setPosition(const Pixel& pos) {
		auto p = glm::vec3(pos.x, pos.y, _position[2]);
		_setPosition(p);
	}

	void Camera::move(const Vec2& delta) {
		_translate(glm::vec3{ delta.x, delta.y, 0.0f });
	}

	void Camera::setZoom(const Vec2& zoom) {
		_zoom = zoom;
		_recalculateProjection();
	}

	glm::vec3 Camera::getPosition() const {
		return _position;
	}

	void Camera::pitch(const float& theta) {
		_theta += theta;
		_view = glm::rotate(theta * glm::pi<float>() / 180.0f, glm::vec3(1, 0, 0)) * _view;
		_recalculateViewProjection();
	}

	void Camera::yaw(const float& phi) {
		_phi += phi;
		_view = glm::rotate(phi * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)) * _view;
		_recalculateViewProjection();
	}

	void Camera::roll(const float& psi) {
		_psi += psi;
		_view = glm::rotate(psi * glm::pi<float>() / 180.0f, glm::vec3(0, 0, 1)) * _view;
		_recalculateViewProjection();
	}


	void Camera::resetIsChanged() {
		_isChanged = false;
	}

	bool Camera::isChanged() const {
		return _isChanged;
	}



	const glm::mat4& Camera::getViewProjection() const{
		return _viewProjection;
	}

	glm::mat4 Camera::getViewProjectionLocked() {
		SDL_AtomicLock(&_cameraLock);
		glm::mat4 vp = _viewProjection;
		SDL_AtomicUnlock(&_cameraLock);
		return vp;
	}


	bool Camera::isInViewportRect(const Rect& rect) const {

		return rect.intersects(_viewportRect);

	}



	void Camera::worldToLens(Pixel& p) const {
		auto aligned = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
		_pixelToGl(p, aligned);
		aligned = _viewProjection * aligned;
		_glToPixel(aligned, p);
	}

	void Camera::lensToWorld(Pixel& p) const {
		auto aligned = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
		_pixelToGl(p, aligned);
		aligned = _viewProjectionInverse * aligned;
		_glToPixel(aligned, p);
	}

	void Camera::getVertices(Rect& rect, std::vector<GLfloat>& values) {

		auto p = Pixel(rect.x, rect.y);
		auto gl = glm::vec4{};
		_pixelToGl(p, gl);

		GLfloat x1 = gl.x;
		GLfloat y1 = gl.y;

		p.x = rect.x + rect.w;
		p.y = rect.y + rect.h;
		_pixelToGl(p, gl);

		GLfloat x2 = gl.x;
		GLfloat y2 = gl.y;

		values.clear();
		values.push_back(x1);
		values.push_back(y1);
		values.push_back(x2);
		values.push_back(y1);
		values.push_back(x2);
		values.push_back(y2);
		values.push_back(x1);
		values.push_back(y2);

	}

	//private

	void Camera::_pixelToGl(const Pixel& p, glm::vec4& glPos) const {
		glPos.x = p.x - (_windowDimensions.w / 2.0f);
		glPos.y = p.y - (_windowDimensions.h / 2.0f);
		glPos.z = 0.0f;
		glPos.w = 1.0f;
	}

	void Camera::_glToPixel(const glm::vec4& glPos, Pixel& p) const {
		p.x = glPos.x + (_windowDimensions.w / 2.0f);
		p.y = glPos.y + (_windowDimensions.h / 2.0f);
	}


	void Camera::_translate(glm::vec3&& delta) {
		_position += delta;
		_view = glm::translate(_view, -1.0f * delta);
		_recalculateViewProjection();
	}

	void Camera::_setPosition(const glm::vec3& position) {
		_view = glm::translate(_view, _position - position);
		_recalculateViewProjection();
		_position = position;
	}

	glm::vec3 Camera::_right() const {
		return glm::vec3(_view[0][0], _view[1][0], _view[2][0]);
	}

	glm::vec3 Camera::_up() const {
		return glm::vec3(_view[0][1], _view[1][1], _view[2][1]);
	}

	glm::vec3 Camera::_out() const {
		return glm::vec3(_view[0][2], _view[1][2], _view[2][2]);
	}

	void Camera::_recalculateView() {

		_recalculateViewProjection();
	}

	void Camera::_recalculateProjection() {
		_projection = glm::ortho(-0.5f * _zoom.x * _windowDimensions.w, 0.5f *  _zoom.x * _windowDimensions.w, -0.5f *  _zoom.y * _windowDimensions.h, 0.5f * _zoom.y * _windowDimensions.h);
		_recalculateViewProjection();
	}

	void Camera::_recalculateViewProjection() {

		SDL_AtomicLock(&_cameraLock);
		_viewProjection = _projection * _view;
		_viewProjectionInverse = glm::inverse(_viewProjection);
		_isChanged = true;
		auto botLeft = Pixel{};
		auto topRight = Pixel{ _windowDimensions.w, _windowDimensions.h };
		lensToWorld(botLeft);
		lensToWorld(topRight);
		_viewportRect.x = botLeft.x;
		_viewportRect.y = botLeft.y;
		_viewportRect.w = topRight.x - botLeft.x;
		_viewportRect.h = topRight.y - botLeft.y;
		SDL_AtomicUnlock(&_cameraLock);
	}

	

	
	


	

	

	

	





	


}