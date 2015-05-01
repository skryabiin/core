#ifndef CORE_CAMERA_2D_HPP
#define CORE_CAMERA_2D_HPP

#include <SDL.h>
#include <GL/glew.h>
#include "Templates.hpp"
#include "Config.hpp"
#include "Geometry.hpp"
#include "SDLUtil.hpp"
#include "Event.hpp"


namespace core {




	class Camera2d : public initializable<Camera2d, void, void, void, void> {

	public:

		bool createImpl();
		bool initializeImpl();

		bool resetImpl();
		bool destroyImpl();


		void setWorldCenterPosition(Point p);
		
		Point worldCenterPosition() const;
		
		void setWorldScale(Vec2 scale);
		
		void setViewportRect(SDL_Rect& viewport);

		Vec2 worldScale() const;
		/*

		bool positionDrawable(Drawable& drawable, SimpleRect<int>& rectified);

		*/

		bool Camera2d::positionPoints(std::vector<Pixel>& pixels, std::vector<GLfloat>& vertices);

		bool Camera2d::positionRect(SDL_Rect& rect, std::vector<GLfloat>& vertices);


		void positionRect(SDL_Rect& rect);

		void positionRect(SimpleRect<int>& rect, Vec2& scale);

	private:
		SDL_Rect _bufferRect;
		Point _worldCenterPosition;
		Vec2 _worldScale;	
		int _windowHeight;
		int _windowWidth;
		SDL_Rect _viewportRect;
	};


} //end namespace core

#endif