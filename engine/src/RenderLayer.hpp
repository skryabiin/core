#ifndef CORE_RENDER_LAYER_HPP
#define CORE_RENDER_LAYER_HPP

#include "Templates.hpp"
#include "Color.hpp"
#include "Drawable.hpp"

namespace core {

	struct RenderLayer : public pausable<RenderLayer>, public initializable<RenderLayer, short, void, void, void> {

		bool createImpl(short layerId1);
		bool initializeImpl();
		bool resetImpl();
		bool destroyImpl();

		void pauseImpl();
		void resumeImpl();

		ColorTransform colorTransform;
		short layerId;
		std::vector<Drawable> drawables;
		short minZIndex;
	};


} //end namespace core
#endif