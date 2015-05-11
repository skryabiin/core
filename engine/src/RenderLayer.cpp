#include "RenderLayer.hpp"

namespace core {

	bool RenderLayer::createImpl(short layerId1) {
		layerId = layerId1;
		return true;
	}

	bool RenderLayer::initializeImpl() { return true;  }
	bool RenderLayer::resetImpl() {
		drawables.clear();
		return true;
	}
	bool RenderLayer::destroyImpl() { return true;	}

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