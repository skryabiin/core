#ifndef CORE_CAMERA_HPP
#define CORE_CAMERA_HPP

#include "glm.hpp"
#include <GL/glew.h>
#include "Templates.hpp"
#include "Geometry.hpp"

namespace core {


	class Camera :public equal_comparable<Camera>, public initializable<Camera, void, void, void, void> {
	public:
		Camera();

		//initializable
		//-------------
		bool createImpl();

		bool initializeImpl(); //requires renderer window dimensions defined

		bool resetImpl();

		bool destroyImpl();
		

		
		//position
		void setPosition(const Pixel& p); //sets the camera's position absolutely
		
		void move(const Vec2& delta);  //moves the camera relatively

		void setZoom(const Vec2& zoom); //sets the zoom absolutely.  can be used as "stretch" if the x,y components of zoom are not the same

		glm::vec3 getPosition() const;  //returns the camera's position



		//rotation
		void pitch(const float& theta); //rotates along x-axis (nod your head "yes")

		void yaw(const float& phi); //rotates along z-axis (shake your head "no")

		void roll(const float& psi); //do a barrel roll



		//change tracking
		void resetIsChanged();		

		bool isChanged() const;  //set if the view/projection matrix has changed



		//view projection access
		const glm::mat4& getViewProjection() const;  //returns a reference to the view/projection matrix

		glm::mat4 getViewProjectionLocked(); //returns a copy of the view/projection matrix, obtained during a thread lock



		//viewport projection
		bool isInViewportRect(const SDL_Rect& rect) const;  //returns true if the rectangle (in world coordinates) is on the screen
		
		void worldToLens(Pixel& p) const; //projects a world coordinate to the surface of the camera lens (the screen)
			
		void lensToWorld(Pixel& p) const; //projects a point on the surface of the camera lens (the screen) into the world

		void getVertices(SDL_Rect& rect, std::vector<GLint>& values); //outputs projected world opengl vertices for the supplied rectangle


	private:
		
		//pixel coordinate alignment
		void _pixelToGl(const Pixel& p, glm::vec4& glPos) const;

		void _glToPixel(const glm::vec4& glPos, Pixel& p) const;



		//position
		void _translate(glm::vec3&& delta);  //uses glm::translate

		void _setPosition(const glm::vec3& position); 

		glm::vec3 _position;

		Vec2 _zoom;

		

		//rotation
		float _theta;

		float _phi;

		float _psi;



		//axis vectors
		glm::vec3 _right() const;

		glm::vec3 _up() const;

		glm::vec3 _out() const;


		
		//camera matrices				
		void _recalculateView();
		
		void _recalculateProjection();
		
		void _recalculateViewProjection();

		glm::mat4 _view;

		glm::mat4 _viewInverse;

		glm::mat4 _projection;

		glm::mat4 _viewProjection;



		//camera projection of screen rectangle
		SDL_Rect _viewportRect;



		//window dimensions used for calculating ortho
		Dimension _windowDimensions;




		//change tracking
		bool _isChanged;
	
		SDL_SpinLock _cameraLock;





		

		

		
	};


} //end namespace core

#endif