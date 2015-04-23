#ifndef CORE_SDLUTIL_H
#define CORE_SDLUTIL_H

#include <SDL.h>
#include "Geometry.hpp"

namespace core {


	inline SDL_Rect simpleRectToSdlWindow(SimpleRect<int> rect, int windowHeight) {
		auto out = SDL_Rect{};
		out.x = rect.x1;
		out.y = windowHeight - rect.y1 - rect.height();
		out.w = rect.width();
		out.h = rect.height();

		return out;
	}

	inline SDL_Rect simpleRectToSdl(SimpleRect<int> rect) {

		auto out = SDL_Rect{};
		out.x = rect.x1;
		out.y = rect.y1;
		out.w = rect.width();
		out.h = rect.height();
		return out;
	}


	inline SDL_Color colorToSdl(Color c) {
		auto out = SDL_Color{};
		out.a = c.a;
		out.r = c.r;
		out.g = c.g;
		out.b = c.b;
		return out;
	}

} //end namespace core

#endif