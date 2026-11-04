#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "config.h"

#define GL_SILENCE_DEPRECATION

#define ZERO_PLUS_FLOAT_TOLERANCE 1e-6
#define PixelArr unsigned char*

inline float degrees2radians(float degrees) {
	return degrees / 180 * M_PI;
}

inline int mod(int value, int max) {
	return (value % max + max) % max;
}

struct RGBAColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	RGBAColor()
		: r(255), g(0), b(0), a(255) {}

	RGBAColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
		: r(r), g(g), b(b), a(a) {}

	static RGBAColor from_float(float r, float g, float b, float a = 255) {
		return {(unsigned char)std::max(0.0f, r), (unsigned char)std::max(0.0f, g), (unsigned char)std::max(0.0f, b), (unsigned char)std::max(0.0f, a)};
	}
	static RGBAColor from_float(float value) {
		return {(unsigned char)std::max(0.0f, value), (unsigned char)std::max(0.0f, value), (unsigned char)std::max(0.0f, value), 255};
	}

	static RGBAColor random() {
		return RGBAColor{
			(unsigned char)(rand() % 256),
			(unsigned char)(rand() % 256),
			(unsigned char)(rand() % 256),
			255
		};
	};

	void dump() {
		std::cout << "RGBAColor(" << r << ", " << g << ", " << b << ", " << a << ")" << std::endl;
	}

	inline RGBAColor operator+(RGBAColor value) {
		return {
			(unsigned char)(std::min(255, (r + (int)value.r))),
			(unsigned char)(std::min(255, (g + (int)value.g))),
			(unsigned char)(std::min(255, (b + (int)value.b))),
		};
	}
	inline RGBAColor operator-(RGBAColor value) {
		return {
			(unsigned char)(std::max(255, (r - (int)value.r))),
			(unsigned char)(std::max(255, (g - (int)value.g))),
			(unsigned char)(std::max(255, (b - (int)value.b))),
		};
	}
	inline RGBAColor operator*(float value) {
		return {
			(unsigned char)(std::min(255.0f, (r * value))),
			(unsigned char)(std::min(255.0f, (g * value))),
			(unsigned char)(std::min(255.0f, (b * value))),
		};
	}
	inline RGBAColor operator*(RGBAColor value) {
		return {
			(unsigned char)(std::min(255, (r * (int)value.r / 255))),
			(unsigned char)(std::min(255, (g * (int)value.g / 255))),
			(unsigned char)(std::min(255, (b * (int)value.b / 255))),
		};
	}
	inline RGBAColor operator/(float value) {
		return {
			(unsigned char)(r / value),
			(unsigned char)(g / value),
			(unsigned char)(b / value),
		};
	}
	
	void draw_this(PixelArr pixels, int i) {
		int index = i*4;
		pixels[index+0] = r;
		pixels[index+1] = g;
		pixels[index+2] = b;
		pixels[index+3] = a;
	}
	static void draw_rgb(PixelArr pixels, int i, unsigned char r, unsigned char g, unsigned char b) {
		int index = i*4;
		pixels[index+0] = r;
		pixels[index+1] = g;
		pixels[index+2] = b;
		pixels[index+3] = 255;
	}
};

struct Vec2
{
	float x, y;

	inline Vec2 operator*(int scalar) const {
		return {x * scalar, y * scalar};
	}
	inline Vec2 operator/(int scalar) const {
		return {x / scalar, y / scalar};
	}
	inline Vec2 operator+(Vec2 vec) const {
		return {x + vec.x, y + vec.y,};
	}
	inline Vec2 operator-(Vec2 vec) const {
		return {x - vec.x, y - vec.y};
	}
	inline float magnitude() const {
		return sqrtf(x*x + y*y);
	}
	inline float dot(Vec2 vec) {
		return x * vec.x + y * vec.y;
	}
	inline float cross_magnitude(Vec2 vec) {
		return (x * vec.y) - (y * vec.x);
	} 
	void dump() {
		std::cout << "Vec2(" << x << ", " << y << ")" << std::endl;
	}
	void plot(Vec2 source, const int width, const int height, PixelArr pixels, RGBAColor col) {
		int x0 = (int)source.x;
		int y0 = (int)source.y;
		int x1 = (int)(source.x + x);
		int y1 = (int)(source.y + y);
		int dx = std::abs(x1 - x0);
		int dy = -std::abs(y1 - y0);
		int sx = x0 < x1 ? 1 : -1;
		int sy = y0 < y1 ? 1 : -1;
		int err = dx + dy;

		while (true) {
			if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height) {
				int i = (y0 * width + x0);
				col.draw_this(pixels, i);
			}

			if (x0 == x1 && y0 == y1) break;
			int e2 = 2 * err;
			if (e2 >= dy) {
				err += dy;
				x0 += sx;
			}
			if (e2 <= dx) {
				err += dx;
				y0 += sy;
			}
		}
	}
};

struct Vec3 {
	float x, y, z;
	// TODO should be const Vec3&
	inline Vec3 operator*(float scalar) const {
		return {x * scalar, y * scalar, z * scalar};
	}
	inline Vec3 operator*(Vec3 scalar) const { // eig kein operator
		return {x * scalar.x, y * scalar.y, z * scalar.z};
	}
	inline Vec3 operator/(float scalar) const {
		return {x / scalar, y / scalar, z / scalar};
	}
	inline Vec3 operator+(Vec3 vec) const {
		return {x + vec.x, y + vec.y, z + vec.z};
	}
	inline Vec3 operator-(Vec3 vec) const {
		return {x - vec.x, y - vec.y, z - vec.z};
	}
	inline float dot(Vec3 vec) {
		return x * vec.x + y * vec.y + z * vec.z;
	}
	inline Vec3 cross(const Vec3& vec) const {
		return {
			y * vec.z - z * vec.y,
			z * vec.x - x * vec.z,
			x * vec.y - y * vec.x
		};
	}
	inline float& operator[](const size_t index) {
		if (index > 2) throw "Vector index out of bounds";
		return *(&x + index);
	}
	inline float square_magnitude() {
		return x * x + y * y + z * z;
	}
	inline float magnitude() {
		return sqrt(square_magnitude());
	}
	inline RGBAColor to_color() {
		return RGBAColor::from_float(x*255, y*255, z*255);
	}

	void rotate(Vec3 rotation) {
		float alpha = rotation.x;
		float beta  = rotation.y;
		float gamma = rotation.z;

		// --- Rotate around Y (yaw = beta) ---
		float x1 =  x * cos(beta) + z * sin(beta);
		float y1 =  y;
		float z1 = -x * sin(beta) + z * cos(beta);

		// --- Rotate around X (pitch = alpha) ---
		float x2 =  x1;
		float y2 =  y1 * cos(alpha) - z1 * sin(alpha);
		float z2 =  y1 * sin(alpha) + z1 * cos(alpha);

		// --- Rotate around Z (roll = gamma) ---
		float x3 =  x2 * cos(gamma) - y2 * sin(gamma);
		float y3 =  x2 * sin(gamma) + y2 * cos(gamma);
		float z3 =  z2;

		// Store result
		x = x3;
		y = y3;
		z = z3;
	}

	void rotate_order_reversed(Vec3 rotation) {
		float alpha = rotation.x;
		float beta  = rotation.y;
		float gamma = rotation.z;

		// --- Rotate around Z ---
		float x1 = x * cos(gamma) - y * sin(gamma);
		float y1 = x * sin(gamma) + y * cos(gamma);
		float z1 = z;
		// --- Rotate around X ---
		float x2 = x1;
		float y2 = y1 * cos(alpha) - z1 * sin(alpha);
		float z2 = y1 * sin(alpha) + z1 * cos(alpha);
		// --- Rotate around Y ---
		float x3 = x2 * cos(beta) + z2 * sin(beta);
		float y3 = y2;
		float z3 = -x2 * sin(beta) + z2 * cos(beta);

		// Store result
		x = x3;
		y = y3;
		z = z3;
	}
	void dump() {
		std::cout << "Vec3(" << x << ", " << y << ", " << z << ")" << std::endl;
	}

	Vec2 flatten() {
		Vec2 vec = {x, y};
		return vec;
	}
};

struct Triangle2d {
	Vec2 vertices[3];
	RGBAColor col;
	Vec3 per_vertex_normals[3];
	Vec2 upper_bound, lower_bound;
	float square_center_dist;
	
	void calc_bounding_box() {
		upper_bound = {-INFINITY, -INFINITY};
		lower_bound = {INFINITY, INFINITY};
		for (int i = 0; i < 3; i++)
		{
			upper_bound.x = std::max(upper_bound.x, vertices[i].x);
			upper_bound.y = std::max(upper_bound.y, vertices[i].y);
			lower_bound.x = std::min(lower_bound.x, vertices[i].x);
			lower_bound.y = std::min(lower_bound.y, vertices[i].y);
		}
	}

	inline bool point_in_bounding_box(Vec2& point) {
		return (point.x >= lower_bound.x && point.x <= upper_bound.x &&
				point.y >= lower_bound.y && point.y <= upper_bound.y);
	}

	bool did_point_hit(Vec2& point) {
		int results[3] = {0,0,0};
		for (int i=0; i<3; i++) {
			// basic formula:
			// (v2-v1) * normal(p-v1) < 0
			// >> is normal of edge pointing in the same direction as the conection vector

			// finde edge
			Vec2& v1 = vertices[i];
			Vec2& v2 = vertices[(i+1) % 3];

			// exclude back facing triangles by only allowing CCW winded triangles
			float value = (v1.y-v2.y) * (point.x-v1.x) + (v2.x-v1.x) * (point.y-v1.y);
			if (value < ZERO_PLUS_FLOAT_TOLERANCE) return false;
		}
	return true;
	}

	Vec3 barycentric_coordinates(Vec2& point) {
		Vec2 AB = vertices[1] - vertices[0];
		Vec2 BC = vertices[2] - vertices[1];
		Vec2 CA = vertices[0] - vertices[2];

		float tri_area2 = AB.cross_magnitude(BC); // triangle_size * 2

		Vec2 AP = point - vertices[0];
		Vec2 BP = point - vertices[1];
		Vec2 CP = point - vertices[2];

		float AB_P_area2 = AB.cross_magnitude(AP);
		float BC_P_area2 = BC.cross_magnitude(BP);
		float CA_P_area2 = CA.cross_magnitude(CP);

		Vec3 result = {
			BC_P_area2 / tri_area2,
			CA_P_area2 / tri_area2,
			AB_P_area2 / tri_area2,
		};
		return result;
	}

	inline void dump() {
		std::cout << "Triangle2d(v1,v2,v2,pvn1,pvn2,pvn3,ub,lb) {" << std::endl;
		vertices[0].dump();
		vertices[1].dump();
		vertices[2].dump();
		per_vertex_normals[0].dump();
		per_vertex_normals[1].dump();
		per_vertex_normals[2].dump();
		upper_bound.dump();
		lower_bound.dump();
		std::cout << "}" << std::endl;
	}
};

struct Triangle {
	Vec3 vertices[3];
	Vec3 normal;
	RGBAColor col;
	Vec3 per_vertex_normals[3];

	inline Triangle2d flatten() { // * unused -> (for orthographic projection)
		return {{
			vertices[0].flatten(),
			vertices[1].flatten(),
			vertices[2].flatten()
		},
		col
		};
	}

	inline Vec3 get_center() {
		return {
			(vertices[0].x + vertices[1].x + vertices[2].x) / 3,
			(vertices[0].y + vertices[1].y + vertices[2].y) / 3,
			(vertices[0].z + vertices[1].z + vertices[2].z) / 3,
		};
	}

	inline void dump() {
		std::cout << "Triangle(v1,v2,v2,n,pvn1,pvn2,pvn3) {" << std::endl;
		vertices[0].dump();
		vertices[1].dump();
		vertices[2].dump();
		normal.dump();
		per_vertex_normals[0].dump();
		per_vertex_normals[1].dump();
		per_vertex_normals[2].dump();
		std::cout << "}" << std::endl;
	}
};

inline void print(int val) {std::cout << "int: " << val << std::endl;}
inline void print(size_t val) {std::cout << "size: " << val << std::endl;}
inline void print(float val) {std::cout << "float: " << val << std::endl;}
inline void print(const char* val) {std::cout << "string: " << val << std::endl;}
inline void print(RGBAColor val) {val.dump();}
inline void print(Vec2 val) {val.dump();}
inline void print(Vec3 val) {val.dump();}
inline void print(Triangle2d val) {val.dump();}
inline void print(Triangle val) {val.dump();}