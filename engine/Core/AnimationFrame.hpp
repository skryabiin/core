#ifndef CORE_ANIMATION_FRAME_HPP
#define CORE_ANIMATION_FRAME_HPP

#include "Geometry.hpp"

namespace core {

	struct AnimationFrame {

		AnimationFrame() : 
			ticksSinceLast{ 0 }, 
			startingFrame{ false }, 
			endingPoint{ false }, 
			endingGoto{ -1 }, 
			terminationPoint{ false }, 
			colorId{ -1 } {
			
		};


		int delay;
		int ticksSinceLast;
		bool startingFrame;
		bool terminationPoint;
		bool endingPoint;
		int endingGoto;
		SDL_Rect frame;
		int colorId;
	};



} //end namespace core

#endif