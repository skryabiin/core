#include "Interpolation.hpp"

namespace core {


	Interpolation::Interpolation(float* target_) : elapsedTotal{ 0 }, target{ target_ } {

	}

	bool Interpolation::update(float dt) {
		elapsedTotal += dt;
		lastValue = function(elapsedTotal);
		*target = lastValue.value;
		return lastValue.done;
	}

	LinearInterpolation::LinearInterpolation(float* target_, float begin_, float end_, float duration_) : Interpolation{ target_ } {		
		lastValue.value = begin_;

		function = [=](float elapsedTotal) -> InterpolationValue {
			auto out = InterpolationValue{};
			if (elapsedTotal >= duration_) {
				elapsedTotal = duration_;
				out.done = true;
			}
			out.value = (end_ - begin_) * (elapsedTotal / duration_) + begin_;
			return out;
		};

	}


} //end namespace core