#ifndef CORE_ANIMATION_HPP
#define CORE_ANIMATION_HPP


#include <map>
#include <vector>
#include "Templates.hpp"
#include "Geometry.hpp"
#include "Event.hpp"
#include "Entity.hpp"
#include "AnimationFrame.hpp"

namespace core {

	class Texture;





	struct Animation : public equal_comparable<Animation>, public nameable {

		Animation() : 
			currentFrameIndex{ 0 }, 
			finishedStart{ false }, 
			finishedEnd{ false }, 
			firstLoopIndex{ -1 }, 
			firstEndIndex{ -1 } {
			

		};

		Texture* texture;
		std::vector<AnimationFrame> frames;
		bool finishedStart;
		bool finishedEnd;
		int currentFrameIndex;
		int firstLoopIndex;
		int firstEndIndex;

		void start();
		void end();
		bool update(int dt);

		Vec2 orientation;


	};





} //end namespace core

#endif
