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

		auto& lua = single<Core>().lua();
		_worldScale = Vec2{ lua("Config")["camera"]["scale"][1], lua("Config")["camera"]["scale"][2] };

		_windowWidth = lua("Config")["window"]["dimensions"][1];
		_windowHeight = lua("Config")["window"]["dimensions"][2];
		_worldCenterPosition.x = lua("Config")["camera"]["centerWorldPoint"][1];
		_worldCenterPosition.y = lua("Config")["camera"]["centerWorldPoint"][2];
		_viewportRect.w = _windowWidth;
		_viewportRect.h = _windowHeight;

		_view = glm::mat4();
		_projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -11.0f, 11.0f);
		_viewProjection = _projection * _view;
		_fov = 95.0f;
		_aspectRatio = 1.0;
		_minVisionRange = 0.0f;
		_maxVisionRange = 100.0f;
		_theta = 0;
		_phi = 0;
		_position = glm::vec3(0.0f,0.0f, 0.0f);

		_target = glm::vec3(0.0f, 0.0f, -1.0f);
		//_recalculateProjection();
		lockOnTarget();
		disengageTarget();

		return true;

	}
	bool Camera::resetImpl() {
		return true;
	}
	bool Camera::destroyImpl() {
		return true;
	}



	void Camera::_recalculateView() {

		_recalculateViewProjection();
	}

	void Camera::positionRect(Rect& rect) {

	}

	

	//apparently glm is in radians even though GLM_FORCE_RADIANS is not defined
	void Camera::_recalculateProjection() {
		_projection = glm::perspective(_fov * glm::pi<float>() / 180.0f, _aspectRatio, _minVisionRange, _maxVisionRange);
		_recalculateViewProjection();
	}


	void Camera::_recalculateViewProjection() {

		SDL_AtomicLock(&_cameraLock);
		_viewProjection = _projection * _view;
		_isChanged = true;
		SDL_AtomicUnlock(&_cameraLock);
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

	void Camera::setPosition(const Pixel& pos) {

		float x = pos.x / (_windowWidth / 2.0f);
		float y = pos.y / (_windowHeight / 2.0f);

		_position = glm::vec3{ x, y, _position[2] };
	}
	void Camera::pitch(const float& theta) {		
		_lockOn = false;
		_view = glm::rotate(theta * glm::pi<float>() / 180.0f, glm::vec3(1, 0, 0)) * _view;
		_recalculateViewProjection();
	}

	void Camera::yaw(const float& phi) {		
		_lockOn = false;		
		_view = glm::rotate(phi * glm::pi<float>() / 180.0f, glm::vec3(0, 1, 0)) * _view;
		_recalculateViewProjection();
	}

	void Camera::roll(const float& psi) {
		_lockOn = false;
		_view = glm::rotate(psi * glm::pi<float>() / 180.0f, glm::vec3(0, 0, 1)) * _view;
		_recalculateViewProjection();
	}

	void Camera::moveOut(const float& delta) {
		translate(-delta * getOut());
	}

	void Camera::moveRight(const float& delta) {
		translate(-delta * getRight());
	}

	void Camera::moveUp(const float& delta) {
		translate(-delta * getUp());
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


	const glm::mat4& Camera::getViewProjection() const{
		return _viewProjection;
	}

	glm::mat4 Camera::getViewProjectionLocked() {
		SDL_AtomicLock(&_cameraLock);
		glm::mat4 vp = _viewProjection;
		SDL_AtomicUnlock(&_cameraLock);
		return vp;
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


	Pixel Camera::alignPoint(const Pixel p) {
		auto aligned = glm::vec4{0.0f,0.0f,0.0f,1.0f};
		//align things to the world center and scale to GL coordinates
		aligned.x = (p.x - _worldCenterPosition.x) / (_windowWidth / 2.0f);
		aligned.y = (p.y - _worldCenterPosition.y) / (_windowHeight / 2.0f);

		aligned = aligned * _viewProjection;
		auto out = Pixel{};

		out.x = roundFloat(aligned.x * (_windowWidth / 2.0f) + _worldCenterPosition.x);
		out.y = roundFloat(aligned.y * (_windowHeight / 2.0f) + _worldCenterPosition.y);
		return out;
	}

	void Camera::getVertices(Rect& rect, std::vector<GLfloat>& values) {
		auto rectAligned = Rect{};
		//align things to the world center
		rectAligned.x = rect.x - _worldCenterPosition.x;
		rectAligned.y = rect.y - _worldCenterPosition.y;

		//now scale to gl coordinates
		rectAligned.x = rectAligned.x / (_windowWidth / 2.0f);
		rectAligned.y = rectAligned.y / (_windowHeight / 2.0f);
		rectAligned.w = rect.w / (_windowWidth / 2.0f);
		rectAligned.h = rect.h / (_windowHeight / 2.0f);

		//now map out coordinates
		GLfloat x1 = rectAligned.x;
		GLfloat x2 = rectAligned.x + rectAligned.w;
		GLfloat y1 = rectAligned.y;
		GLfloat y2 = rectAligned.y + rectAligned.h;

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

}