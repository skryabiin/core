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

		if (_facets.reset() != InitStatus::CREATE_TRUE) return false;

		return RenderableSystem2d::resetImpl();

	}

	bool AnimationSystem2d::destroyImpl() {

		if (_facets.destroy() != InitStatus::CREATE_FALSE) return false;

		return RenderableSystem2d::destroyImpl();
	}

	void AnimationSystem2d::updateDrawablePosition(VisualFacet* vfacet) {

	}

	bool AnimationSystem2d::handleEvent(AnimationChangeEvent& animationChange) {

	
		auto facet = _facets.getFacet(animationChange.facetId);
		if (facet == nullptr) return true;

			if (facet->currentAnimation.name().compare(animationChange.nextAnimation))  {

				if (animationChange.endImmediate) {
					facet->currentAnimation = facet->animationSet.getAnimation(animationChange.nextAnimation);
					facet->startedAnimation = true;
				}
				else {
					facet->nextAnimation = animationChange.nextAnimation;
					facet->endAnimation = true;
				}
					
			}

			return false;

	}

	AnimationFacet* AnimationSystem2d::createAnimationFacet(Entity& e) {

		auto newFacet = AnimationFacet{};

		newFacet.setOf(e);

		return _facets.addFacet(newFacet);		

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

		return _facets.getBaseFacets(e);

	}

	void AnimationSystem2d::updateImpl(float dt, RuntimeContext& context) {
		_facets.forEach([&](AnimationFacet* facet)->void {
			if (facet->isPaused()) return;
			auto didChangeFrames = facet->currentAnimation.update(context.dt);
			if (didChangeFrames || facet->startedAnimation) {

				facet->drawable.sourceRect = facet->currentAnimation.frames[facet->currentAnimation.currentFrameIndex].frame;
				//single<Renderer>().updateDrawable(facet.drawable);
				facet->startedAnimation = false;
			}
		});
	}



	void AnimationSystem2d::destroyFacets(Entity& e) {

		_facets.removeFacets(e);
		
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
			
			auto facet = system->createAnimationFacet(entity);
			auto animationSet = single<TextureManager>().getAnimationSet(animationSetName);

			facet->animationSet = animationSet;

			facet->currentAnimation = animationSet.getAnimation(currentAnimation);

			facet->offset = offset.getPixel();

			facet->drawable.camera = system->camera();

			facet->drawable.texture = facet->currentAnimation.texture;

			facet->drawable.sourceRect = facet->currentAnimation.frames[facet->currentAnimation.currentFrameIndex].frame;
			facet->drawable.layerId = system->getDrawableLayerId();
			facet->drawable.targetRect.w = roundFloat(facet->drawable.sourceRect.w * facet->scale.x);
			facet->drawable.targetRect.h = roundFloat(facet->drawable.sourceRect.h * facet->scale.y);
			facet->drawable.targetRect.x = position[0] + facet->offset.x;
			facet->drawable.targetRect.y = position[1] + facet->offset.y - facet->drawable.targetRect.h;
			facet->drawable.zIndex = position[2];
			//facet.drawable.id = single<Renderer>().createDrawable(facet.drawable);

			lua.pushStack(facet->id());
		}
		else {
			lua.pushStack(-1L);
		}

		

		return 1;
	}


} //end namespace core