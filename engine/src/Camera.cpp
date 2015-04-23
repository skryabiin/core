#include "Camera.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtx/transform.hpp"
#include "gtx/rotate_vector.hpp"

namespace core {

	Camera::Camera() {
		_view = glm::mat4();
		_projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		_fov = 75.0f;
		_aspectRatio = 4.0f / 3.0f;
		_minVisionRange = 10.0f;
		_maxVisionRange = 100.0f;
		_theta = 0;
		_phi = 0;
		_position = glm::vec3(0);
	}

	void Camera::_recalculateView() {

		_recalculateViewProjection();
	}


	//apparently glm is in radians even though GLM_FORCE_RADIANS is not defined
	void Camera::_recalculateProjection() {
		_projection = glm::perspective(_fov * glm::pi<float>() / 180.0f, _aspectRatio, _minVisionRange, _maxVisionRange);
		_recalculateViewProjection();
	}


	void Camera::_recalculateViewProjection() {
		_viewProjection = _projection * _view;
		_isChanged = true;
	}

	void Camera::resetIsChanged() {
		_isChanged = false;
	}

	bool Camera::isChanged() const {
		return _isChanged;
	}

	void Camera::lockOnTarget() {
		_view = glm::lookAt(_position, _target, glm::vec3(0, 1, 0));
		_recalculateViewProjection();
	}

	void Camera::setTarget(const glm::vec3& target) {
		_target = target;
	}

	void Camera::disengageTarget() {
		_lockOn = false;
	}

	void Camera::setPosition(const glm::vec3& position) {
		_position = position;
		if (_lockOn) {
			lockOnTarget();
		}
		else {
			_view = glm::translate(_view, position - _position);			
			_recalculateViewProjection();
		}
	}


	void Camera::pitch(const float& theta) {		
		_lockOn = false;
		_view = glm::rotate(theta, glm::vec3(1, 0, 0)) * _view;
		_recalculateViewProjection();
	}

	void Camera::yaw(const float& phi) {		
		_lockOn = false;		
		_view = glm::rotate(phi, glm::vec3(0, 1, 0)) * _view;		
		_recalculateViewProjection();
	}

	void Camera::roll(const float& psi) {
		_lockOn = false;
		_view = glm::rotate(psi, glm::vec3(0, 0, 1)) * _view;
		_recalculateViewProjection();
	}

	void Camera::moveOut(const float& delta) {
		translate(delta * getOut());
	}

	void Camera::moveRight(const float& delta) {
		translate(delta * getRight());
	}

	void Camera::moveUp(const float& delta) {
		translate(delta * getUp());
	}


	glm::vec3 Camera::getPosition() const {
		return _position;
	}

	glm::vec3 Camera::getRight() const {
		return glm::vec3(_view[0][0], _view[1][0], _view[2][0]);
	}

	glm::vec3 Camera::getUp() const {
		return glm::vec3(_view[0][1], _view[1][1], _view[2][1]);
	}

	glm::vec3 Camera::getOut() const {
		return glm::vec3(_view[0][2], _view[1][2], _view[2][2]);
	}

	void Camera::translate(const glm::vec3& delta) {
		_position += delta;
		if (_lockOn) {
			lockOnTarget();
		}
		else {
			_view = glm::translate(_view, delta);			
			_recalculateViewProjection();
		}
	}

	void Camera::setFovDegrees(const float& theta) {
		_fov = theta;
		_recalculateProjection();
	}

	void Camera::setAspectRatio(const float& ratio) {
		_aspectRatio = ratio;
		_recalculateProjection();
	}

	void Camera::setMinVisionRange(const float& range) {
		_minVisionRange = range;
		_recalculateProjection();
	}

	void Camera::setMaxVisionRange(const float& range) {
		_maxVisionRange = range;
		_recalculateProjection();
	}


	const glm::mat4& Camera::getViewProjection() const {
		return _viewProjection;
	}

	float Camera::getFovDegrees() const {
		return _fov;
	}

	float Camera::getAspectRatio() const {
		return _aspectRatio;
	}

	float Camera::getMinVisionRange() const {
		return _minVisionRange;
	}

	float Camera::getMaxVisionRange() const {
		return _maxVisionRange;
	}

}