#include "AnimationSystem2d.hpp"
#include "Renderer.hpp"
#include "Templates.hpp"
#include "Core.hpp"


namespace core {

	AnimationSystem2d::AnimationSystem2d() {


	}



	InitStatus AnimationSystem2d::initializeImpl() {


		std::function<void(AnimationSystem2d*, AnimationChangeEvent&)> callback = std::mem_fn(&AnimationSystem2d::handleAnimationChange);

		_animationChangeFilter.init(this, callback);
		single<EventProcessor>().addFilter(&_animationChangeFilter);

		return RenderableSystem2d::initializeImpl();
	}

	void AnimationSystem2d::handleFacetPauseEvent(FacetPauseEvent& pauseEvent) {		

		for (auto& facet : _animationFacets) {
			if (facet.of() == pauseEvent.entity) {

				//doing all of the facets for this entity so continue
				if (pauseEvent.facetId == -1) {

					if (pauseEvent.paused) {
						facet.pause();
						single<Renderer>().pauseDrawable(facet.drawable);
					}
					else {
						facet.resume();
						single<Renderer>().resumeDrawable(facet.drawable);
					}
					continue;
				}

				//doing just the one facet for this entity so stop
				else if (pauseEvent.facetId == facet.id()) {
					if (pauseEvent.paused) {
						facet.pause();
						single<Renderer>().pauseDrawable(facet.drawable);
					}
					else {
						facet.resume();
						single<Renderer>().resumeDrawable(facet.drawable);
					}
					continue;
					break;
				}

			}
		}

	}

	void AnimationSystem2d::handleAnimationChange(AnimationChangeEvent& animationChange) {

	

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

				return;

			}

		}

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

	void AnimationSystem2d::updateImpl(RuntimeContext& context) {

		for (auto& facet : _animationFacets) {
			if (facet.isPaused()) continue;
			auto didChangeFrames = facet.currentAnimation.update(context.dt);
			if (didChangeFrames || facet.startedAnimation) {				

				facet.drawable.sourceRect = facet.currentAnimation.frames[facet.currentAnimation.currentFrameIndex].frame;
				single<Renderer>().updateDrawable(facet.drawable);
				facet.startedAnimation = false;
			}
		}
	}


	void AnimationSystem2d::updateDrawPosition(PositionChangeEvent& positionChange) {

		//check if this is the entity the camera is centered on
		

		auto moveIt = _movingAnimations.find(positionChange.entity);

		AnimationFacet* animation = nullptr;

		if (moveIt == std::end(_movingAnimations)) {
			for (unsigned i = 0; i < _animationFacets.size(); i++) {
				if (_animationFacets[i].of() == positionChange.entity) {
					animation = &_animationFacets[i];
					break;
				}
			}
			if (animation == nullptr) return;

			_movingAnimations.insert(std::pair<Entity, AnimationFacet*>(positionChange.entity, animation));

		}
		else {
			animation = moveIt->second;
		}



		auto p = positionChange.position.getPixel();
		if (_cameraFollow.of() == positionChange.entity && !_cameraFollow.isPaused()) {			
			auto w = animation->drawable.targetRect.w;
			auto h = animation->drawable.targetRect.h;
			auto x = p.x + w * 0.5f;
			auto y = p.y + h * 0.5f;
			snapCameraToCoordinates(x, y);
		}

		int x1 = p.x + animation->offset.x;
		int y1 = p.y + animation->offset.y;

		animation->drawable.targetRect.x = x1;
		animation->drawable.targetRect.y = y1 + animation->drawable.targetRect.h;

		animation->drawable.zIndex = p.z + animation->offset.z;


	}

	//TODO these have to do the right thing
	InitStatus AnimationSystem2d::resetImpl() {

		single<EventProcessor>().removeFilter(&_animationChangeFilter);

		for (auto& facet : _animationFacets) {
			single<Renderer>().destroyDrawable(facet.drawable);
		}

		_movingAnimations.clear();		
		_animationFacets.clear();
		_animationFacets.shrink_to_fit();

		return RenderableSystem2d::resetImpl();
		
	}

	void AnimationSystem2d::destroyFacets(Entity& e) {

		auto movingIt = _movingAnimations.find(e);
		if (movingIt != std::end(_movingAnimations)) {
			_movingAnimations.erase(movingIt);
		}

		for (auto it = std::begin(_animationFacets); it != std::end(_animationFacets); ++it) {
			if (it->of() == e) {
				single<Renderer>().destroyDrawable(it->drawable);
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
			auto animationSet = single<ResourceManager>().getAnimationSet(animationSetName);

			facet.animationSet = animationSet;

			facet.currentAnimation = animationSet.getAnimation(currentAnimation);

			facet.offset = offset.getPixel();

			facet.drawable.entityId = entity;
			
			facet.drawable.camera = system->getCamera();

			facet.drawable.texture = facet.currentAnimation.texture;

			facet.drawable.sourceRect = facet.currentAnimation.frames[facet.currentAnimation.currentFrameIndex].frame;
			facet.drawable.layerId = system->getDrawableLayerId();
			facet.drawable.targetRect.w = roundFloat(facet.drawable.sourceRect.w * facet.scale.x);
			facet.drawable.targetRect.h = roundFloat(facet.drawable.sourceRect.h * facet.scale.y);
			facet.drawable.targetRect.x = position[0] + facet.offset.x;
			facet.drawable.targetRect.y = position[1] + facet.offset.y - facet.drawable.targetRect.h;
			facet.drawable.zIndex = position[2];
			facet.drawable.id = single<Renderer>().createDrawable(facet.drawable);

			lua.pushStack(facet.id());
		}
		else {
			lua.pushStack(-1L);
		}

		

		return 1;
	}

	EventProcessor::EventRegistration<AnimationChangeEvent> animationChangeEventReg{};


} //end namespace core