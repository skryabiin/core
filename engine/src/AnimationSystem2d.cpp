#include "AnimationSystem2d.hpp"
#include "Renderer.hpp"
#include "Templates.hpp"
#include "Core.hpp"


namespace core {

	AnimationSystem2d::AnimationSystem2d() {

	}


	bool AnimationSystem2d::createImpl() {
		if (!RenderableSystem2d::createImpl()) return false;
		return true;
	}

	bool AnimationSystem2d::initializeImpl() {

		if (!RenderableSystem2d::initializeImpl()) return false;


		return true;
	}

	//TODO these have to do the right thing
	bool AnimationSystem2d::resetImpl() {

		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::DESTROY_DRAWABLE;
		dc.layerId = _drawableLayerId;
		for (auto& facet : _animationFacets) {
			dc.facetId = facet.id();
			single<Renderer>().applyDrawableChange(dc);
		}

		_movingAnimations.clear();
		_animationFacets.clear();
		_animationFacets.shrink_to_fit();

		return RenderableSystem2d::resetImpl();

	}

	bool AnimationSystem2d::destroyImpl() {
		return RenderableSystem2d::destroyImpl();
	}

	void AnimationSystem2d::updateDrawablePosition(VisualFacet* vfacet) {

	}

	bool AnimationSystem2d::handleEvent(AnimationChangeEvent& animationChange) {

	

		for (auto& animation : _animationFacets) {

			if (animation.of() == animationChange.entity) {

				if (animation.currentAnimation.name().compare(animationChange.nextAnimation))  {

					if (animationChange.endImmediate) {
						animation.currentAnimation = animation.animationSet.getAnimation(animationChange.nextAnimation);
						animation.startedAnimation = true;
					}
					else {
						animation.nextAnimation = animationChange.nextAnimation;
						animation.endAnimation = true;
					}
					
				}

				return true;

			}

		}
		return true;

	}

	AnimationFacet& AnimationSystem2d::createAnimationFacet(Entity& e) {

		auto newFacet = AnimationFacet{};

		newFacet.setOf(e);

		_animationFacets.push_back(newFacet);

		return _animationFacets.back();

	}

	/*

	void AnimationSystem2d::renderImpl() {

		auto d = Drawable{};

		for (auto& animation : _animationFacets) {

			auto f = animation.currentAnimation.frames[animation.currentAnimation.currentFrameIndex];

			if (animation.isPaused() || (_currentColorId >= 0 && f.colorId >= 0 && f.colorId != _currentColorId)) continue;

			d.texture = animation.currentAnimation.texture;
			d.sourceRect = f.frame;
			d.targetRect = SDL_Rect{animation.position.x, animation.position.y, f.frame.w, f.frame.h};
			d.scale = animation.scale;			
			single<Renderer>().render(d, _camera);

		}
	} */

	std::vector<Facet*> AnimationSystem2d::getFacets(Entity& e) {

		auto out = std::vector<Facet*>{};

		for (auto& facet : _animationFacets) {
			out.push_back(&facet);
		}

		return out;

	}

	void AnimationSystem2d::updateImpl(float dt, RuntimeContext& context) {

		for (auto& facet : _animationFacets) {
			if (facet.isPaused()) continue;
			auto didChangeFrames = facet.currentAnimation.update(context.dt);
			if (didChangeFrames || facet.startedAnimation) {				

				facet.drawable.sourceRect = facet.currentAnimation.frames[facet.currentAnimation.currentFrameIndex].frame;
				//single<Renderer>().updateDrawable(facet.drawable);
				facet.startedAnimation = false;
			}
		}
	}



	void AnimationSystem2d::destroyFacets(Entity& e) {

		auto movingIt = _movingAnimations.find(e);
		if (movingIt != std::end(_movingAnimations)) {
			_movingAnimations.erase(movingIt);
		}

		auto dc = DrawableChange{};
		dc.operation = DrawableChange::Operation::DESTROY_DRAWABLE;
		dc.layerId = _drawableLayerId;

		for (auto it = std::begin(_animationFacets); it != std::end(_animationFacets); ++it) {
			if (it->of() == e) {
				
				dc.facetId = it->id();
				single<Renderer>().applyDrawableChange(dc);				
				it = _animationFacets.erase(it);
				break;
			}
		}
		
	}

	int AnimationSystem2d::createFacet_bind(LuaState& lua) {

		info("Dumping lua stack in AnimationSystem2d: ",lua.printStack());

		auto systemName = lua.pullStack<std::string>(1);
		auto entity = lua.pullStack<int>(2);
		


		auto system = single<Core>().getSystemByName<AnimationSystem2d>(systemName);

		if (system != nullptr) {

			LuaPixel position = lua["position"];

			
			LuaVec2 scale = lua["scale"];
			
			LuaPixel offset = lua["offset"];

			std::string currentAnimation = lua["currentAnimation"];
			

			std::string animationSetName = lua["animationSetName"];
			
			auto& facet = system->createAnimationFacet(entity);
			auto animationSet = single<TextureManager>().getAnimationSet(animationSetName);

			facet.animationSet = animationSet;

			facet.currentAnimation = animationSet.getAnimation(currentAnimation);

			facet.offset = offset.getPixel();

			facet.drawable.camera = system->camera();

			facet.drawable.texture = facet.currentAnimation.texture;

			facet.drawable.sourceRect = facet.currentAnimation.frames[facet.currentAnimation.currentFrameIndex].frame;
			facet.drawable.layerId = system->getDrawableLayerId();
			facet.drawable.targetRect.w = roundFloat(facet.drawable.sourceRect.w * facet.scale.x);
			facet.drawable.targetRect.h = roundFloat(facet.drawable.sourceRect.h * facet.scale.y);
			facet.drawable.targetRect.x = position[0] + facet.offset.x;
			facet.drawable.targetRect.y = position[1] + facet.offset.y - facet.drawable.targetRect.h;
			facet.drawable.zIndex = position[2];
			//facet.drawable.id = single<Renderer>().createDrawable(facet.drawable);

			lua.pushStack(facet.id());
		}
		else {
			lua.pushStack(-1L);
		}

		

		return 1;
	}


} //end namespace core