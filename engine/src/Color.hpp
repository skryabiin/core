#ifndef CORE_COLOR_HPP
#define CORE_COLOR_HPP

#include "glm.hpp"
#include "GL/glew.h"

namespace core {


	struct Color {

		enum class CommonColor {

			RED,
			BLUE,
			GREEN,
			WHITE,
			BLACK,
			CLEAR
		};

		Color() : r{ 0 }, g{ 0 }, b{ 0 }, a{ 0 } {};

		Color(CommonColor color) {
			auto common = get(color);
			r = common.r;
			g = common.g;
			b = common.b;
			a = common.a;
		}

		Color(const Color& other) {
			r = other.r;
			g = other.g;
			b = other.b;
			a = other.a;
		}

		Color(const glm::vec4& vec) : r{ vec[0] }, g{ vec[1] }, b{ vec[2] }, a{ vec[3] } {}

		Color(GLfloat rr, GLfloat gg, GLfloat bb, GLfloat aa) : r{ rr }, g{ gg }, b{ bb }, a{ aa } {};

		Color(unsigned short rr, unsigned short gg, unsigned short bb, unsigned short aa) : r{ rr / 255.0f }, g{ gg / 255.0f }, b{ bb / 255.0f }, a{ aa / 255.0f } {};

		friend bool operator==(const Color& a, const Color& b) {
			return (a.r == b.r) && (a.g == b.g) && (a.b == b.g) && (a.a == b.a);
		}

		friend bool operator!=(const Color& a, const Color& b) {
			return !(a == b);
		}

		operator glm::vec4() const {
			return glm::vec4{ r, g, b, a };
		}

		GLfloat r, g, b, a;

		GLfloat& operator[](int index) {
			switch (index) {
			case 0:
				return r;
				break;
			case 1:
				return b;
				break;
			case 2:
				return g;
				break;
			case 3:
				return a;
				break;
			default:
				return r;
			}
		}

		

		static Color get(CommonColor color) {
			switch (color) {
			case Color::CommonColor::RED:
				return Color{ 1.0f, 0.0f, 0.0f, 1.0f };
				break;
			case Color::CommonColor::GREEN:
				return Color{ 0.0f, 1.0f, 0.0f, 1.0f };
				break;
			case Color::CommonColor::BLUE:
				return Color{ 0.0f, 0.0f, 1.0f, 1.0f };
				break;
			case Color::CommonColor::WHITE:
				return Color{ 1.0f, 1.0f, 1.0f, 1.0f };
				break;
			case Color::CommonColor::BLACK:
				return Color{ 0.0f, 0.0f, 0.0f, 1.0f };
				break;
			case Color::CommonColor::CLEAR:
				return Color{ 0.0f, 0.0f, 0.0f, 0.0f };
				break;
			}

			return Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		}

	};


	struct ColorTransform {

		ColorTransform() : overrideSystem{ false } {}

		ColorTransform(const glm::mat4& matrix) {
			transform = matrix;
		}

		ColorTransform(const GLfloat* matrix) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					transform[j][i] = matrix[i * 4 + j];
				}
			}
		}

		~ColorTransform() {}

		glm::mat4 transform;

		bool overrideSystem;

		operator glm::mat4() {
			return transform;
		}

		ColorTransform operator+(const ColorTransform& other) {
			auto out = ColorTransform{};
			out.transform = this->transform + other.transform;
			return out;
		}

		ColorTransform operator-(const ColorTransform& other) {
			auto out = ColorTransform{};
			out.transform = this->transform - other.transform;
			return out;
		}

		ColorTransform operator*(float scalar) {
			auto out = ColorTransform{};
			out.transform = this->transform * scalar;
			return out;
		}

		Color operator*(const Color& color) {
			return transform * (glm::vec4)color;
		}

		ColorTransform operator*(const ColorTransform& other) {
			auto out = ColorTransform{};
			out.transform = this->transform * other.transform;
			return out;
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


	

} //end namespace core

#endif