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

} //end namespace core