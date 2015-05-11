#ifndef CORE_CAMERA_HPP
#define CORE_CAMERA_HPP

#include "glm.hpp"
#include <GL/glew.h>
#include "Templates.hpp"
#include "Geometry.hpp"

namespace core {


	class Camera : public initializable<Camera, void, void, void, void> {
	public:
		Camera();

		bool createImpl();
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		void positionRect(Rect& rect);

		void getVertices(Rect& rect, std::vector<GLfloat>& values);

		void lockOnTarget();

		void setTarget(const glm::vec3& target);

		void disengageTarget();

		void setPosition(const glm::vec3& position);

		void pitch(const float& theta);

		void yaw(const float& phi);

		void roll(const float& psi);

		void translate(const glm::vec3& delta);

		void setFovDegrees(const float& theta);

		void setAspectRatio(const float& ratio);

		void setMinVisionRange(const float& range);
		
		void setMaxVisionRange(const float& range);

		void resetIsChanged();

		void moveOut(const float& delta);

		void moveRight(const float& delta);

		void moveUp(const float& delta);

		glm::vec3 getOut() const;

		glm::vec3 getRight() const;

		glm::vec3 getUp() const;

		glm::vec3 getPosition() const;

		float getFovDegrees() const;

		float getAspectRatio() const;
		
		float getMinVisionRange() const;

		float getMaxVisionRange() const;

		bool isChanged() const;

		const glm::mat4& getViewProjection() const;

		void setOrthoProjection(float minx, float maxx, float miny, float maxy);

	private:

		bool _isChanged;
		glm::vec3 _position;

		bool _lockOn;
		glm::vec3 _target;
		glm::mat4 _view;
		glm::mat4 _projection;

		glm::mat4 _viewProjection;

		glm::vec3 _facing;

		float _theta;
		float _phi;
		float _fov;
		float _aspectRatio;
		float _minVisionRange;
		float _maxVisionRange;

		void _recalculateView();
		void _recalculateProjection();
		void _recalculateViewProjection();

		Point _worldCenterPosition;
		Vec2 _worldScale;
		int _windowHeight;
		int _windowWidth;
		Rect _viewportRect;


	};


} //end namespace core

#endif