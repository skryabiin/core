#ifndef CORE_ANIMATION_FACET_HPP
#define CORE_ANIMATION_FACET_HPP

#include <string>

#include "Facet.hpp"
#include "AnimationSet.hpp"
#include "Animation.hpp"
#include "Geometry.hpp"

namespace core {

	struct AnimationFacet : public Facet {

		AnimationFacet() : startedAnimation{ false }, endAnimation{ false }, nextAnimation{ "" }, scale{ 1.0f, 1.0f } {
			setDeclaredTypeInfo(&typeid(*this));
		}

		AnimationSet animationSet;
		Animation currentAnimation;
		std::string nextAnimation;
		bool endAnimation;
		bool startedAnimation;
		Pixel offset;
		Vec2 scale;

		int drawableId;
	};




} //end namespace core
#endif
