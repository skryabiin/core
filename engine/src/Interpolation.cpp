#include "Interpolation.hpp"

namespace core {


	Interpolation::Interpolation(float duration_) {
		duration = duration_;
	}


	InterpolationValue Interpolation::getValue(float elapsed) {
		return function(elapsed);
	}


	LinearInterpolation::LinearInterpolation(float duration_) : Interpolation(duration_) {
		

		function = [=](float elapsedTotal) -> InterpolationValue {
			auto out = InterpolationValue{};
			if (elapsedTotal >= duration_) {
				out.value = 1;
				out.done = true;
				return out;							
			}
			out.value = elapsedTotal / duration_;
			return out;
		};

	}



} //end namespace core