#ifndef CORE_MATH_H
#define CORE_MATH_H

#include <cmath>
#include <cfloat>
#include <cstddef>
#include <limits>
#include <iostream>
#include <sstream>



namespace core {
#define FLOAT_EPSILON 0.00001f
	
	template <typename T>
	inline T randInRange(const T& start, const T& end) {
		if (end - start < FLOAT_EPSILON) return start;
		return start + static_cast <T>(rand()) / (static_cast<float>(RAND_MAX / (end - start)));
	}
	template<>
	inline int randInRange(const int& start, const int& end) {
		if (start == end) return start;
		return start + (rand() % (end - start));
	}

	template <typename T>
	inline T bounded(const T& value, const T& lowerBound, const T& upperBound) {
		if (value < lowerBound) return lowerBound;
		if (value > upperBound) return upperBound;
		return value;
	}

	inline void setBit1(unsigned& arr, int at) {
		arr |= (1 << at);
	}
	inline void setBit0(unsigned& arr, int at) {
		arr &= ~(1 << at);
	}

	inline void setBit(unsigned& arr, unsigned expr, int at) {
		(expr) ? setBit1(arr, at) : setBit0(arr, at);
	}

	inline unsigned bit1In(unsigned& arr, int from, int to) {
		return (arr >> from) << (31 - to + from);
	}

	inline unsigned bitAt(unsigned& arr, int at) {
		return arr & (1 << at);
	}

	inline unsigned inRange(int& i, int& j, int& k) {
		return (i <= j) ? ((k >= i && k <= j) ? 1 : 0) : ((k >= j && k <= i) ? 1 : 0);
	}

	inline unsigned inRangeSorted(int& i, int& j, int& k, int &l) {
		return (l < i || k > j) ? 0 : 1;
	}

	inline unsigned inRange(int& i, int& j, int& k, int& l) {
		return inRangeSorted((i <= j) ? i : j, (i <= j) ? j : i, (k <= l) ? k : l, (k <= l) ? l : k);

	}

	inline bool inRangeSorted(const float& i, const float& j, const float& k, const float &l) {
		return (l < i || k > j) ? 0 : 1;
	}

	inline bool inRange(const float& i, const float& j, const float& k, const float& l) {
		return inRangeSorted((i <= j) ? i : j, (i <= j) ? j : i, (k <= l) ? k : l, (k <= l) ? l : k);

	}

	inline unsigned inRange(float& i, float& j, float& k) {
		return (i <= j) ? ((k >= i && k <= j) ? 1 : 0) : ((k >= j && k <= i) ? 1 : 0);
	}


	inline bool inRect(int x, int y, int rx, int ry, int rx2, int ry2) {		
		return ((inRange(rx, rx2, x)) & (inRange(ry, ry2, y))) == 1;
	}

	inline std::string printBits(unsigned number) {
		std::ostringstream ss;

		for (auto i = 0; i < 32; i++) {
			int bit = number % 2;
			ss << (number % 2);
			number >>= 1;
		}

		return ss.str();
	}


	//common math functions`
	inline int mod(int a, int b) {
		auto c = a % b;
		c = (c < 0) ? b + c : c;
		return c;
	}

	inline int signOf(int x) {
		if (x == 0) {
			return 0;
		}
		return (x > 0) ? 1 : -1;
	}


	inline
		int signOf(float x) {
			if (fabs(x) < FLOAT_EPSILON) {
				return 0;
			}
			return (x > 0.0f) ? 1 : -1;
		}


	inline float infinity() {
		return std::numeric_limits<float>::infinity();
	}

	inline bool isValid(float x)
	{
		if (x != x)
		{
			// NaN.
			return false;
		}

		return -1.0f * infinity() < x && x < infinity();
	}

	inline bool magGt(float a, float b)
	{
		return(fabs(a) > fabs(b));
	}

	inline float magLargest(float a, float b)
	{
		return magGt(a, b) ? a : b;
	}

	inline float cmMagSmallest(float a, float b)
	{
		return magGt(a, b) ? b : a;
	}

	inline bool ptCmp(float x1, float y1, float x2, float y2) {
		return (fabs(x1 - x2) < FLOAT_EPSILON && fabs(y1 - y2) < FLOAT_EPSILON);
	}

	inline float distance(float x1, float y1, float x2, float y2) {
		return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	}

	inline float distance(float x1, float y1, float z1, float x2, float y2, float z2) {
		return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - x1, 2));
	}

	inline bool floatCmp(float x1, float x2) {
		if (x1 == infinity() && x2 == infinity()) return true;
		if (x1 == infinity() || x2 == infinity()) return false;
		return (fabs(x1 - x2) < FLOAT_EPSILON);
	}

	template<typename T>
	bool isPowerOfTwo(T& x)
	{
		return ((x != 0) && ((x & (~x + 1)) == x));
	}

	template <typename T>
	T nextPowerOf2(T& input)
	{
		T value = 1;

		while (value < input) {
			value <<= 1;
		}
		return value;
	}

//disabling rounding loss of information warnings since that's the point
#pragma warning( disable: 4244 )

	inline int roundFloat(float x) {
		return (int)(x >= 0) ? x + 0.5f : x - 0.5f;
	}

#pragma warning ( default: 4424 )


} //end namespace core

#endif
