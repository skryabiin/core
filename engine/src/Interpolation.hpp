#ifndef CORE_INTERPOLATION_HPP
#define CORE_INTERPOLATION_HPP

#include <functional>

namespace core {

	
	struct InterpolationValue {

		InterpolationValue() : value{ 0 }, done{ false } {};
		float value;
		bool done;
	};
	
	struct Interpolation{

		Interpolation(float* target_);

		bool update(float dt);

		InterpolationValue lastValue;

		float elapsedTotal;

		float* target;

		std::function<InterpolationValue(float)> function;
		
	};

	struct LinearInterpolation : public Interpolation {

		LinearInterpolation(float* target_, float begin_, float end_, float duration_);
	};

} //end namespace core
#endif