#include "RenderLayer.hpp"
#include "Renderer.hpp"

namespace core {

	bool RenderLayer::createImpl(short layerId1) {
		layerId = layerId1;
		frame.create(single<Renderer>().windowDimensions());
		return true;
	}

	bool RenderLayer::initializeImpl() { 
		
		if (frame.initialize() != InitStatus::INIT_TRUE) {
			return false;
		}
		return true;  
	
	}
	bool RenderLayer::resetImpl() {
		bool check = (frame.reset() != InitStatus::CREATE_TRUE);		
		drawables.clear();
		return check;
	}
	bool RenderLayer::destroyImpl() { 		
		bool check = (frame.destroy() != InitStatus::CREATE_FALSE);
		return check;
	}

	void RenderLayer::pauseImpl() {}
	void RenderLayer::resumeImpl() {}

	short RenderLayer::minZIndex() const {
		return drawables.back().zIndex;
	}

	Drawable* RenderLayer::addDrawable(Drawable& d) {
		auto insIt = std::end(drawables);

		for (auto it = std::begin(drawables); it != std::end(drawables); ++it) {
			if (d.zIndex > it->zIndex) {
				insIt = it;
				break;
			}
		}
		auto out = drawables.insert(insIt, d);
		return &(*out);					
	}

	void RenderLayer::destroyDrawable(int facetId) {
		for (auto it = std::begin(drawables); it != std::end(drawables); ++it) {
			if (it->facetId == facetId) {
				drawables.erase(it);
				break;
			}
		}
	}

} //end namespace core