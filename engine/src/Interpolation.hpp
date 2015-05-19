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


		Interpolation(float duration_);

		std::function<InterpolationValue(float)> function;		

		float duration;

		InterpolationValue getValue(float elapsed);

	};

	struct LinearInterpolation : public Interpolation {

		LinearInterpolation(float duration_);
	};


} //end namespace core
#endif