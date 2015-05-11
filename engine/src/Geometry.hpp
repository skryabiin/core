#ifndef CORE_GEOMETRY_HPP
#define CORE_GEOMETRY_HPP

#include <cmath>
#include <vector>
#include <sstream>
#include "Math.hpp"

namespace core {




	/// A 2D column vector.
	struct Vec2
	{

		/// Default constructor does nothing (for performance).
		Vec2() : x{ 0.0f }, y{ 0.0f } {}


		// copy constructor
		Vec2(const Vec2& other) : x(other.x), y(other.y) {};


		/// Construct using coordinates.
		Vec2(float x, float y) : x(x), y(y) {}


		//make a copy
		Vec2 copyVec() {
			return Vec2(this->x, this->y);
		}

		/// Set this vector to all zeros.
		void setZero() { x = 0.0f; y = 0.0f; }

		///check if this vector is zero
		bool isZero() {
			return (fabs(x) < FLOAT_EPSILON && fabs(y) < FLOAT_EPSILON);
		}

		bool isInfinite() {
			return isValid(x) && isValid(y);
		}

		/// Set this vector to some specified coordinates.
		void set(float x_, float y_) { x = x_; y = y_; }

		/// Negate this vector.
		Vec2 operator -() const { Vec2 v; v.set(-x, -y); return v; }

		//add two vectors and return a third
		Vec2 operator +  (const Vec2& v) const {
			return Vec2(this->x + v.x, this->y + v.y);
		}

		//subtract a vector and return a third
		Vec2 operator - (const Vec2& v) const {
			return Vec2(this->x - v.x, this->y - v.y);
		}

		//multiply the vector by a scalar
		Vec2 operator * (float c) const {
			return Vec2(this->x * c, this->y * c);
		}

		/// Read from and indexed element.
		float operator () (int i) const
		{
			return (&x)[i];
		}

		/// Write to an indexed element.
		float& operator () (int i)
		{
			return (&x)[i];
		}

		//compare both object equality
		//and value equality
		bool operator ==(const Vec2& v)
		{
			if (this == &v) {
				return true;
			}
			return (*this + v).isZero();
		}
		/// Add a vector to this vector.
		void operator += (const Vec2& v)
		{
			x += v.x; y += v.y;
		}

		/// Subtract a vector from this vector.
		void operator -= (const Vec2& v)
		{
			x -= v.x; y -= v.y;
		}

		/// Multiply this vector by a scalar.
		void operator *= (float a)
		{
			x *= a; y *= a;
		}

		Vec2& operator = (const Vec2& v)
		{
			x = v.x;
			y = v.y;
			return *this;
		}

		//dot product
		float operator * (const Vec2& v) {
			return x * v.x + y * v.y;
		}

		//scale vector by component
		Vec2 scale(const Vec2& v) {
			return Vec2(x * v.x, y * v.y);
		}

		//not sure what this is, so call it mash
		//projects this to orthogonal of unit vector v
		void mashcross(const Vec2& unitV) {
			x *= unitV.y;
			y *= unitV.x;
		}

		void mash(const Vec2& unitV) {
			x *= unitV.x;
			y *= unitV.y;
		}

		/// Get the length of this vector (the magnitude).
		float len() const
		{
			return sqrt(x * x + y * y);
		}

		/// Get the length squared
		float lenSq() const
		{
			return x * x + y * y;
		}

		/// Get the absolute value vector
		Vec2 absVec() const {
			return Vec2(fabs(x), fabs(y));
		}

		/// Convert this vector into a unit vector
		Vec2 unitVec() const
		{
			auto length = len();
			auto nx = x;
			auto ny = y;
			if (length < FLOAT_EPSILON)
			{
				nx = infinity();
				ny = infinity();

			}
			else {
				nx /= length;
				ny /= length;
			}
			return Vec2(nx, ny);
		}

		//return axis vector based on the greater
		//magnitude of the component values
		Vec2 getPrimaryCardinalProjection() const {
			if (magGt(x, y)) {
				return Vec2(x, 0);
			}
			else {
				return Vec2(0, y);
			}

		}

		/// Get the skew vector such that dot(skew_vec, other) == cross(vec, other)
		Vec2 getSkew() const
		{

			return Vec2(-y, x);
		}

		float x, y;
	};


	inline std::string print(Vec2 v) {
		std::ostringstream ss;
		ss << "vector(" << v.x << ", " << v.y << ")";
		return std::string(ss.str());

	}


	

	struct Dimension {
		int w, h;
		Dimension() : w{ 0 }, h{ 0 } {};
		Dimension(int ww, int hh) : w{ ww }, h{ hh } {};
		Dimension(const Dimension& other) : w{ other.w }, h{ other.h }{};
		Dimension& operator=(const Dimension& other) {
			w = other.w;
			h = other.h;
			return *this;
		}

		bool operator==(const Dimension& other) const {
			return w == other.w && h == other.h;
		}

		bool operator!=(const Dimension& other) const {
			return w != other.w || h != other.h;
		}

	};

	//represents an integer screen pixel location
	//with a z index
	struct Pixel {
		int x, y, z;

		Pixel() : x{ 0 }, y{ 0 }, z{ 0 } {};

		Pixel(int xx, int yy) : x{ xx }, y{ yy }, z{ 0 } {};

		Pixel(int xx, int yy, int zz) : x{ xx }, y{ yy }, z{ zz } {};

		Pixel(const Pixel& other) : x{ other.x }, y{ other.y }, z{ other.z }
		{};

		Pixel& operator=(const Pixel& other) {
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}

		Pixel operator-(const Pixel& other) const {
			return Pixel{ x - other.x, y - other.y, z - other.z };
		}

		Pixel operator+(const Pixel& other) const {
			return Pixel{ x + other.x, y + other.y, z + other.z };
		}

		Pixel& operator+=(const Pixel& other) {
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		Pixel& operator-=(const Pixel& other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		Pixel mask(const Pixel& other) {
			return Pixel{ (other.x == 0) ? 0 : x, (other.y == 0) ? 0 : y, (other.z == 0) ? 0 : z };
		}


		float dist(const Pixel& other) const {
			return distance(x, y, other.x, other.y);
		}

		Pixel& translate(const Vec2 &move) {
			x += roundFloat(move.x);
			y += roundFloat(move.y);
			return *this;
		}

		bool operator==(const Pixel& other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		bool operator!=(const Pixel& other) const {
			return x != other.x || y != other.y || z != other.z;
		}

	};


	struct Point {
		float x, y, z;

		Point() : x{ 0.0f }, y{ 0.0f } {};
		Point(float xx, float yy) : x{ xx }, y{ yy }, z{ 0.0f } {};
		Point(float xx, float yy, float zz) : x{ xx }, y{ yy }, z{ zz } {};

		Point(const Pixel pixel) {
			x = pixel.x;
			y = pixel.y;
			z = pixel.z;
		}
		Point translate(const Vec2 &move) {
			x += move.x;
			y += move.y;
			return *this;
		}


		Point operator*(float scalar) const {
			return Point{ x * scalar, y * scalar, z * scalar };
		}


		Point operator +(const Point other) const {
			return Point{ x + other.x, y + other.y, z + other.z };
		}

		Point operator -(const Point other) const {
			return Point{ x - other.x, y - other.y, z - other.z };
		}

		void operator +=(const Point other) {
			x += other.x;
			y += other.y;
			z += other.z;
		}

		void operator -=(const Point other) {
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}


		float dist2d(const Point& other) const {
			return distance(x, y, other.x, other.y);
		}

		float dist(const Point& other) const {
			return distance(x, y, z, other.x, other.y, other.z);
		}

		bool operator ==(const Point other) const {
			return (fabs(x - other.x) < FLOAT_EPSILON && fabs(y - other.y) < FLOAT_EPSILON) && fabs(z - other.z);
		}

		bool operator !=(const Point other) const {
			return !(*this == other);
		}



	};


	struct Rect {

		float x, y, w, h;

		Rect() : x{ 0.0f }, y{ 0.0f }, w{ 0.0f }, h{ 0.0f } {}
		Rect(float xx, float yy, float ww, float hh) : x{ xx }, y{ yy }, w{ ww }, h{ hh } {}
		Rect(Point p) : x{ p.x }, y{ p.y }, w{ 0.0f }, h{ 0.0f } {}		

		Rect operator *(const Vec2 scale) const {
			auto out = Rect{ x, y, w * scale.x, h * scale.y };
		}

		Point center() const {
			return Point{ x + 0.5f * w, y + 0.5f * h, 0.0f};
		}

		bool intersects(const Rect& other) const {
			bool xaxis = inRangeSorted(x, x + w, other.x, other.x + other.w);
			if (!xaxis) return false;
			return inRangeSorted(y, y + h, other.y, other.y + other.h);
		}

	};

	inline std::string print(Pixel& p) {
		std::ostringstream ss;
		ss << "pixel(" << p.x << ", " << p.y << ", " << p.z << ")";
		return ss.str();
	}

	inline std::string print(Point p) {
		std::ostringstream ss;
		ss << "point(" << p.x << ", " << p.y << ")";
		return std::string(ss.str());
	}


	class Segment {


	public:

		Segment() {};

		Segment(Point aa, Point bb) : a{ aa }, b{ bb } {};
	

		Point a; 
		Point b;

		bool intersects(const Segment &other) const {

			Point isect = intersection(other);
			if (floatCmp(isect.x, infinity())) {
				return false;
			}
			return intersectOnSegment(isect) && other.intersectOnSegment(isect);

		}

		bool intersectOnSegment(const Point isect) const {
			bool isOnSegment = true;			

			if ((b.x > a.x && (isect.x < (a.x - FLOAT_EPSILON) || isect.x >(b.x + FLOAT_EPSILON))) || (b.x <= a.x && (isect.x < (b.x - FLOAT_EPSILON) || isect.x >(a.x + FLOAT_EPSILON)))) {

				isOnSegment = false;
			}

			if ((b.y > a.y && (isect.y < (a.y - FLOAT_EPSILON) || isect.y >(b.y + FLOAT_EPSILON))) || (b.y <= a.y && (isect.y < (b.y - FLOAT_EPSILON) || isect.y >(a.y + FLOAT_EPSILON)))) {

				isOnSegment = false;
			}

			return isOnSegment;
		}


		Point intersection(const Segment &other) const {


			//check for lines in parallel
			float thisSlope = slope();
			float otherSlope = other.slope();
			if (floatCmp(thisSlope, otherSlope)) {
				return Point(infinity(), infinity());
			}
			float thisYIntersect = yIntersect();
			float otherYIntersect = other.yIntersect();
			float x, y;
			if (floatCmp(thisSlope, infinity())) {
				x = a.x;
				y = otherSlope * x + otherYIntersect;
			}
			else if (floatCmp(otherSlope, infinity())) {
				x = other.a.x;
				y = thisSlope * x + thisYIntersect;
			}
			else {
				x = (other.yIntersect() - thisYIntersect) / (thisSlope - otherSlope);
				y = thisSlope * x + thisYIntersect;

			}
			Point oldIsectPoint = Point(x, y);

			return oldIsectPoint;
		}

		float slope() const {
			if (floatCmp(b.x - a.x, 0.0f)) return infinity();
			return (b.y - a.y) / (b.x - a.x);
		}

		float yIntersect() const {
			float thisSlope = slope();
			if (thisSlope == infinity()) return infinity();
			return b.y - thisSlope * b.x;
		}

		Point center() const {
			return Point((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
		}

		float length() const {
			return distance(a.x, a.y, b.x, b.y);
		}

		void translate(Vec2 move) {
			a.translate(move);
			b.translate(move);
		}


	};


	struct Edge : public Segment {

		Edge() : Segment{} {};

		Edge(Point aa, Point bb) : Segment{ aa, bb } {};

		//the vector orthogonal to this edge facing away from the center of the polygon
		Vec2 normalVector;


	};



	struct EdgeIntersect {

		Edge e1;
		Edge e2;
		Point isectPoint;

	};



	template <typename T>
	struct SimpleRect {

		SimpleRect() {};

		SimpleRect(T xx1, T yy1, T xx2, T yy2) {
			x1 = xx1;
			y1 = yy1;
			x2 = xx2;
			y2 = yy2;
		}

		T x1;
		T y1;
		T x2;
		T y2;

		T width() const {
			return (x2 > x1) ? x2 - x1 : x1 - x2;
		}

		T height() const {
			return (y2 > y1) ? y2 - y1 : y1 - y2;
		}

		float boundingRadius() const {
			return distance(x1, y1, x2, y2) / 2;
		}

		Point center() const {
			return Point((x1 + x2) / 2, (y1 + y2) / 2);
		}

		void translate(Vec2 move) {
			x1 += move.x;
			y1 += move.y;
			x2 += move.x;
			y2 += move.y;
		}

		Point getVertex(int i) const {
			switch (i) {
			case 0:
				return Point{ x1, y1 };
			case 1:
				return Point{ x2, y1 };
			case 2:
				return Point{ x2, y2 };
			case 3:
				return Point{ x1, y2 };
			default:
				return Point{};
			}
		}

		std::vector<Edge> getEdges() const {
			auto ab = Edge{ getVertex(0), getVertex(1) };
			auto bc = Edge{ getVertex(1), getVertex(2) };
			auto cd = Edge{ getVertex(2), getVertex(3) };
			auto da = Edge{ getVertex(3), getVertex(0) };

			auto rectCenter = center();
			auto abCenter = ab.center();
			auto bcCenter = bc.center();
			auto cdCenter = cd.center();
			auto daCenter = da.center();

			ab.normalVector = Vec2(abCenter.x - rectCenter.x, abCenter.y - rectCenter.y).unitVec();
			bc.normalVector = Vec2(bcCenter.x - rectCenter.x, bcCenter.y - rectCenter.y).unitVec();
			cd.normalVector = Vec2(cdCenter.x - rectCenter.x, cdCenter.y - rectCenter.y).unitVec();
			da.normalVector = Vec2(daCenter.x - rectCenter.x, daCenter.y - rectCenter.y).unitVec();
			auto out = std::vector<Edge>{};
			out.push_back(ab);
			out.push_back(bc);
			out.push_back(cd);
			out.push_back(da);
			
			return out;
		}

		std::vector<EdgeIntersect> intersections(Edge &other) const {

			auto isects = std::vector<EdgeIntersect>{};

			auto edges = getEdges();
			for (auto& i : edges) {

				if (i.intersects(other)) {
					auto isect = EdgeIntersect{};
					isect.e1 = i;
					isect.e2 = other;
					isect.isectPoint = i.intersection(other);
					isects.push_back(std::move(isect));
				}
			}
			return isects;

		}

		std::vector<EdgeIntersect> intersections(SimpleRect<T> &other) const {

			auto isects = std::vector<EdgeIntersect>{};

			auto edges = getEdges();
			auto otherEdges = other.getEdges();

				EdgeIntersect isect;
				for (auto& i : edges) {

					

					for (auto& j : otherEdges) {

						if (i.intersects(j)) {
							auto isect = EdgeIntersect{};
							isect.e1 = i;
							isect.e2 = j;
							isects.push_back(std::move(isect));
						}
					}
				}


			return isects;

		}

	};



	class Shape {


	};

	/*
	class Rect : public Shape {

	public:

		Rect() {

		}

		Rect(float x, float y, float w, float h) {
			a = Point(x, y);
			b = Point(x + w, y);
			c = Point(x + w, y + h);
			d = Point(x, y + h);
			createEdges();
		}

		Rect(Point aa, Point bb, Point cc, Point dd) {
			a = aa;
			b = bb;
			c = cc;
			d = dd;
			createEdges();
		}

		Rect(const Rect &other) {

			a = Point(other.getA());
			b = Point(other.getB());
			c = Point(other.getC());
			d = Point(other.getD());
			createEdges();
		}


		Rect& operator=(const Rect& other) {

			a = Point(other.getA());
			b = Point(other.getB());
			c = Point(other.getC());
			d = Point(other.getD());
			createEdges();
			return *this;
		}

		Rect& operator=(const Rect&& other) {

			a = Point(other.getA());
			b = Point(other.getB());
			c = Point(other.getC());
			d = Point(other.getD());
			createEdges();
			return *this;
		}

		Point getA() const {
			return a;
		}
		Point getB() const {
			return b;
		}
		Point getC() const {
			return c;
		}

		Point getD() const {
			return d;
		}

		Point* getAPtr() {
			return &a;
		}

		Point* getBPtr() {
			return &b;
		}

		Point* getCPtr() {
			return &c;
		}

		Point* getDPtr() {
			return &d;
		}

		Point center() const {
			float x = (a.x + b.x + c.x + d.x) / 4;
			float y = (a.y + b.y + c.y + d.y) / 4;
			return Point(x, y);
		}

		float boundingRadius() const {
			return distance(a.x, a.y, c.x, c.y) / 2;
		}

		void translate(Vec2 &move) {
			a.translate(move);
			b.translate(move);
			c.translate(move);
			d.translate(move);
		}

		std::vector<EdgeIntersect> intersections(Edge &other) const {

			std::vector<EdgeIntersect> isects;

			for (auto & i : _edges) {

				if (i.intersects(other)) {
					auto isect = EdgeIntersect{};
					isect.e1 = &i;
					isect.e2 = &other;
					isect.isectPoint = i.intersection(other);
					isects.push_back(std::move(isect));
				}
			}
			return isects;

		}

		std::vector<EdgeIntersect> intersections(Rect &other) const {

			std::vector<EdgeIntersect> isects;

			if (center().dist(other.center()) < boundingRadius() + other.boundingRadius()) {

				EdgeIntersect isect;
				for (auto& i : _edges) {

					for (auto& j : other.edges()) {

						if (i.intersects(j)) {
							auto isect = EdgeIntersect{};
							isect.e1 = &i;
							isect.e2 = &j;
							isects.push_back(std::move(isect));
						}
					}
				}

			}


			return isects;

		}



	private:


		

		Point a, b, c, d;

		std::vector<Edge> _edges;


	};

	*/

	inline std::string print(Segment s) {
		std::ostringstream ss;
		ss << "segment: " << print(s.a) << " -- " << print(s.b) << " | length=" << s.length();
		return std::string(ss.str());
	}

	inline std::string print(Edge e) {
		std::ostringstream ss;
		ss << "edge: " << print(e.a) << " -- " << print(e.b) << " | length=" << e.length() << " | normal=" << print(e.normalVector);
		return std::string(ss.str());
	}


} //end namespace core

#endif
