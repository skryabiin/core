#ifndef CORE_LUA_VECTOR_HPP
#define CORE_LUA_VECTOR_HPP

#include "SDL.h"
#include "LuaState.hpp"
#include "LuaTable.hpp"
#include "Color.hpp"

namespace core {



	template <typename T, int dimensions = 2>
	class LuaVector : public LuaTable {
	public:

		LuaVector() : _values{ dimensions } {


			for (unsigned i = 1; i <= dimensions; ++i) {

				lua_reg(i, &_values[i - 1]);
			}
		}

		LuaVector(LuaState& state) : _values{ dimensions } {

			for (unsigned i = 1; i <= dimensions; ++i) {

				lua_reg(i, &_values[i - 1]);
			}

			static_cast<LuaTable>(*this) = state;

		}


		T& operator[](unsigned index) {
			return _values[index];
		}

	protected:
		std::vector<T> _values;
	};


	struct LuaPixel : public LuaVector<int, 3> {

	public:


		LuaPixel() {};

		LuaPixel(const Pixel& p) {
			setPixel(p);
		}

		LuaPixel(LuaState& state) {
			static_cast<LuaTable>(*this) = state;
		}

		Pixel getPixel() const {
			return Pixel{ _values[0], _values[1], _values[2] };
		}

		void setPixel(const Pixel& p) {
			_values[0] = p.x;
			_values[1] = p.y;
			_values[2] = p.z;
		}

	};

	struct LuaDimension : public LuaVector<int, 2> {

		LuaDimension() {};

		LuaDimension(LuaState& state) {
			static_cast<LuaTable>(*this) = state;
		}

		int width() {
			return _values[0];
		}

		int height() {
			return _values[1];
		}

		Dimension getDimension() const {
			return Dimension{ _values[0], _values[1] };
		}

		void setDimension(Dimension dimensions) {
			_values[0] = dimensions.w;
			_values[1] = dimensions.h;
		}

	};

	template<typename T>
	struct LuaRange : public LuaVector<T, 2> {


		int lower() {
			return (_values[0] <= _values[1]) ? _values[0] : _values[1];
		}

		int upper() {
			return (_values[0] >= _values[1]) ? _values[0] : _values[1];
		}



	};

	struct LuaVec2 : public LuaVector<float, 2> {

		LuaVec2() {}

		LuaVec2(LuaState& state) {
			static_cast<LuaTable>(*this) = state;
		}

		Vec2 getVec2() const {
			return Vec2{ _values[0], _values[1] };
		}

		void setVec2(Vec2& v) {
			_values[0] = v.x;
			_values[1] = v.y;
		}
	};

	struct LuaColor : public LuaVector<unsigned short, 4> {
	public:

		LuaColor() {};

		LuaColor(LuaState& state) {
			static_cast<LuaTable>(*this) = state;
		}

		Color getColor() const {
			return Color{ _values[0], _values[1], _values[2], _values[3] };
		}

		void setColor(Color& c) {
			_values[0] = c.r;
			_values[1] = c.g;
			_values[2] = c.b;
			_values[3] = c.a;
		}

	};

	struct LuaRect : public LuaVector<int, 4> {

		LuaRect() {};

		LuaRect(LuaState& state) {
			static_cast<LuaTable>(*this) = state;
		}

		LuaRect(SDL_Rect& sdlRect) {
			setRect(sdlRect);
		}

		SDL_Rect getRect() const {
			return SDL_Rect{ _values[0], _values[1], _values[2], _values[3] };
		}

		void setRect(SDL_Rect& rect) {
			_values[0] = rect.x;
			_values[1] = rect.y;
			_values[2] = rect.w;
			_values[3] = rect.h;
		}


	};


} //end namespace core

#endif