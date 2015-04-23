#ifndef CORE_MATH_H
#define CORE_MATH_H

#include <cmath>
#include <cfloat>
#include <cstddef>
#include <limits>
#include <iostream>
#include <sstream>
#include "glm.hpp"
#include "GL/glew.h"


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

	inline unsigned inRange(float& i, float& j, float& k) {
		return (i <= j) ? ((k >= i && k <= j) ? 1 : 0) : ((k >= j && k <= i) ? 1 : 0);
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


	struct ColorTransform {

		ColorTransform() : overrideSystem{ false } {}

		glm::mat4 transform;

		bool overrideSystem;

		glm::vec4 operator*(const glm::vec4& color) {
			return transform * color;
		}

		void reset() {
			transform = glm::mat4{};
		}

		void greyScale(GLfloat rComponent, GLfloat gComponent, GLfloat bComponent, GLfloat aComponent) {
			for (int i = 0; i < 4; ++i) {
				setChannel(i, rComponent, gComponent, bComponent, aComponent);
			}
		}


		void setAlpha(GLfloat alpha) {
			setAlphaChannel(0, 0, 0, alpha);
		}



		void setRedChannel(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			setChannel(0, r, g, b, a);
		}

		void setGreenChannel(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			setChannel(1, r, g, b, a);
		}

		void setBlueChannel(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			setChannel(2, r, g, b, a);
		}

		void setAlphaChannel(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			setChannel(3, r, g, b, a);
		}

		void setRedInfluence(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			setInfluence(0, r, g, b, a);
		}

		void setGreenInfluence(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			setInfluence(1, r, g, b, a);
		}

		void setBlueInfluence(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			setInfluence(2, r, g, b, a);
		}

		void setAlphaInfluence(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			setInfluence(3, r, g, b, a);
		}

		void setChannel(unsigned short index, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			transform[index][0] = r;
			transform[index][1] = g;
			transform[index][2] = b;
			transform[index][3] = a;

		}

		glm::vec4 operator[](unsigned short index) {
			return transform[index];
		}

		void setInfluence(unsigned short index, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
			transform[0][index] = r;
			transform[1][index] = g;
			transform[2][index] = b;
			transform[3][index] = a;
		}

	};


	struct Color {

		Color() : r{ 0 }, g{ 0 }, b{ 0 }, a{ 0 } {};
		Color(unsigned short rr, unsigned short gg, unsigned short bb, unsigned short aa) : r{ rr }, g{ gg }, b{ bb }, a{ aa } {};

		friend bool operator==(const Color& a, const Color& b) {
			return (a.r == b.r) && (a.g == b.g) && (a.b == b.g) && (a.a == b.a);
		}

		friend bool operator!=(const Color& a, const Color& b) {
			return !(a == b);
		}
		unsigned short r, g, b, a;


		enum class CommonColor {

			RED,
			BLUE,
			GREEN,
			WHITE,
			BLACK,
			CLEAR
		};

		static Color get(CommonColor color) {
			switch (color) {
			case Color::CommonColor::RED:
				return Color(255, 0, 0, 255);
				break;
			case Color::CommonColor::GREEN:
				return Color(0, 255, 0, 255);
				break;
			case Color::CommonColor::BLUE:
				return Color(0, 0, 255, 255);
				break;
			case Color::CommonColor::WHITE:
				return Color(255, 255, 255, 255);
				break;
			case Color::CommonColor::BLACK:
				return Color(0, 0, 0, 255);
				break;
			case Color::CommonColor::CLEAR:
				return Color(0, 0, 0, 0);
				break;
			}

			return Color(255, 255, 255, 255);
		}

	};


	struct SqMatrix2 {

		SqMatrix2() {
			values[0][0] = 0.0f;
			values[0][1] = 0.0f;
			values[1][0] = 0.0f;
			values[1][1] = 0.0f;
		};

		SqMatrix2(float v00, float v01, float v10, float v11) {
			values[0][0] = v00;
			values[0][1] = v01;
			values[1][0] = v10;
			values[1][1] = v11;
		}

		float values[2][2];


		float tr() const {
			return values[0][0] + values[1][1];
		}

		float det() const {
			return values[0][0] * values[1][1] - values[1][0] * values[0][1];
		}

		//add to this matrix
		void operator += (SqMatrix2 other) {
			values[0][0] += other.values[0][0];
			values[0][1] += other.values[0][1];
			values[1][0] += other.values[1][0];
			values[1][1] += other.values[1][1];
		}

		//add matrices
		SqMatrix2 operator + (SqMatrix2 other) const {
			auto out = SqMatrix2{ values[0][0], values[0][1], values[1][0], values[1][1] };
			out += other;
			return out;
		}
		void operator -= (SqMatrix2 other) {
			SqMatrix2 neg = other * -1.0f;
			*this += neg;
		}


		SqMatrix2 operator - (SqMatrix2 other) const {
			auto out = SqMatrix2{ values[0][0], values[0][1], values[1][0], values[1][1] };
			out -= other;
			return out;
		}

		//multiply this matrix by a scalar
		void operator *= (float scalar) {
			values[0][0] *= scalar;
			values[1][0] *= scalar;
			values[0][1] *= scalar;
			values[1][1] *= scalar;
		}

		//evaluate this matrix undder scalar multiplication
		SqMatrix2 operator * (float scalar) const {
			SqMatrix2 out;
			out *= scalar;
			return out;
		}

		//multiply this matrix by another
		void operator *= (SqMatrix2 other) {
			float v00, v01, v10, v11;
			v00 = values[0][0] * other.values[0][0] + values[0][1] * other.values[1][0];
			v01 = values[0][0] * other.values[0][1] + values[0][1] * other.values[1][1];
			v10 = values[1][0] * other.values[0][0] + values[1][1] * other.values[1][0];
			v11 = values[1][0] * other.values[0][1] + values[1][1] * other.values[1][1];
			values[0][0] = v00;
			values[0][1] = v01;
			values[1][0] = v10;
			values[1][1] = v11;
		}

		//evaluate this matrix under multiplication
		SqMatrix2 operator * (SqMatrix2 other) const {
			SqMatrix2 out;
			out *= other;
			return out;
		}


		float at(int i, int j) {
			return values[i][j];
		}



	};

	inline SqMatrix2 identity() {
		return SqMatrix2(1, 0, 0, 1);
	}


	inline void lineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float isect[]) {

		/*
		float x12 = cmSqMatrix2(x1,1.0f,x2,1.0f).det();
		float x34 = cmSqMatrix2(x3,1.0f,x4,1.0f).det();
		float y12 = cmSqMatrix2(y1,1.0f,y2,1.0f).det();
		float y34 = cmSqMatrix2(y3,1.0f,y4,1.0f).det(); */
		auto x12 = x1 - x2;
		auto x34 = x3 - x4;
		auto y12 = y1 - y2;
		auto y34 = y3 - y4;
		auto xy12 = SqMatrix2(x1, y1, x2, y2).det();
		auto xy34 = SqMatrix2(x3, y3, x4, y4).det();

		auto denominator = SqMatrix2(x12, y12, x34, y34).det();

		auto xnumerator = SqMatrix2(xy12, x12, xy34, x34).det();
		auto ynumerator = SqMatrix2(xy12, y12, xy34, y34).det();

		auto xout = (fabs(denominator) < FLOAT_EPSILON) ? infinity() : xnumerator / denominator;
		auto yout = (fabs(denominator) < FLOAT_EPSILON) ? infinity() : ynumerator / denominator;

		std::cout << "line at " << xout << ", " << yout << std::endl;

		isect[0] = xout;
		isect[1] = yout;
	}

} //end namespace core

#endif
