#include "Animation.hpp"
#include "Renderer.hpp"
#include "Entity.hpp"
#include "TextureManager.hpp"

namespace core {





	void Animation::start() {
		currentFrameIndex = 0;	
		frames[currentFrameIndex].ticksSinceLast = 0;
	};

	void Animation::end() {
		if (firstEndIndex >= 0) {
			currentFrameIndex = firstEndIndex;
		}
	}

	bool Animation::update(int dt) {
		
		if (frames.size() < 2 || finishedEnd) return false;


		auto didChangeFrames = false;

		int remainingTicks = dt;

		finishedStart = (currentFrameIndex >= firstLoopIndex);

		while (remainingTicks > 0) {
			AnimationFrame& currentFrame = frames[currentFrameIndex];
			if (currentFrame.delay - currentFrame.ticksSinceLast > remainingTicks) {
				currentFrame.ticksSinceLast += remainingTicks;
				remainingTicks = 0;
			}
			else {
				remainingTicks -= currentFrame.delay - currentFrame.ticksSinceLast;
				currentFrame.ticksSinceLast = 0;				
				currentFrameIndex++;
				didChangeFrames = true;
				//if it's in the loop phase, stop at the first end index and loop around instead
				if (firstEndIndex >= 0) {
					if (currentFrameIndex == firstEndIndex) {
						if (firstLoopIndex >= 0) {
							currentFrameIndex = firstLoopIndex;
						}
					}
				}
				else if (currentFrameIndex == frames.size()) {
					if (firstLoopIndex >= 0) {
						currentFrameIndex = firstLoopIndex;
					}
					else {
						finishedEnd = true;
						currentFrameIndex--;						
					}
				}

			}
		}

		return didChangeFrames;
	}


	AnimationSet::AnimationSet() {


	}

	void AnimationSet::addAnimation(std::string name, Animation animation) {

		_animations[name.c_str()] = animation;

	}

	Animation AnimationSet::getAnimation(std::string name) {

		return _animations[name.c_str()];

	}

	void AnimationSet::setName(std::string name) {
		_name = name;
	}

	std::string AnimationSet::name() const {
		return _name;
	}



} //end namespace core
