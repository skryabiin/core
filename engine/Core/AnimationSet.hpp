#ifndef CORE_ANIMATION_SET_HPP
#define CORE_ANIMATION_SET_HPP

#include "Templates.hpp"
#include "Animation.hpp"

namespace core {

	class AnimationSet : public equal_comparable<AnimationSet> {
	public:

		AnimationSet();

		void setName(std::string name);

		std::string name() const;

		void addAnimation(std::string name, Animation animation);

		Animation getAnimation(std::string name);

	private:

		std::string _name;
		std::map<std::string, Animation> _animations;

	};



} //end namespace core

#endif