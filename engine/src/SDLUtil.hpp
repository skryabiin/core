#ifndef CORE_SDLUTIL_H
#define CORE_SDLUTIL_H

#include <SDL.h>
#include "Color.hpp"


namespace core {


	inline SDL_Color colorToSdl(const Color& c) {
		auto out = SDL_Color{};
		out.a = c.a * 255;
		out.r = c.r * 255;
		out.g = c.g * 255;
		out.b = c.b * 255;
		return out;
	}

} //end namespace core

#endif