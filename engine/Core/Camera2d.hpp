#ifndef CORE_CAMERA_2D_HPP
#define CORE_CAMERA_2D_HPP

#include <SDL.h>
#include "Templates.hpp"
#include "Config.hpp"
#include "Geometry.hpp"
#include "SDLUtil.hpp"
#include "Event.hpp"

namespace core {




	class Camera2d : public initializable<Camera2d, void, void> {

	public:

		InitStatus initializeImpl();

		InitStatus resetImpl();

		void setWorldCenterPosition(Point p);
		
		Point worldCenterPosition() const;
		
		void setWorldScale(Vec2 scale);
		
		Vec2 worldScale() const;
		/*

		bool positionDrawable(Drawable& drawable, SimpleRect<int>& rectified);

		*/
		void positionRect(SDL_Rect& rect);

		void positionRect(SimpleRect<int>& rect, Vec2& scale);

	private:
		Point _worldCenterPosition;
		Vec2 _worldScale;	
		int _windowHeight;
		int _windowWidth;
	};


} //end namespace core

#endif