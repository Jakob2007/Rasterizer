#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime> 

inline float degrees2radians(float degrees) {
	return degrees / 180 * M_PI;
}

struct RGBAColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	RGBAColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
		: r(r), g(g), b(b), a(a) {}
};

struct Vec2
{
	float x, y;

	Vec2 operator*(int scalar) const {
		return {x * scalar, y * scalar};
	}
	Vec2 operator/(int scalar) const {
		return {x / scalar, y / scalar};
	}
	Vec2 operator+(Vec2 vec) const {
		return {x + vec.x, y + vec.y,};
	}
	Vec2 operator-(Vec2 vec) const {
		return {x - vec.x, y - vec.y};
	}
	float dot(Vec2 vec) {
		return x * vec.x + y * vec.y;
	}
	void dump() {
		std::cout << "Vec2(" << x << ", " << y << ")" << std::endl;
	}
};

struct Vec3 {
	float x, y, z;

	Vec3 operator*(float scalar) const {
		return {x * scalar, y * scalar, z * scalar};
	}
	Vec3 operator*(Vec3 scalar) const { // eig kein operator
		return {x * scalar.x, y * scalar.y, z * scalar.z};
	}
	Vec3 operator/(float scalar) const {
		return {x / scalar, y / scalar, z / scalar};
	}
	Vec3 operator+(Vec3 vec) const {
		return {x + vec.x, y + vec.y, z + vec.z};
	}
	Vec3 operator-(Vec3 vec) const {
		return {x - vec.x, y - vec.y, z - vec.z};
	}
	float dot(Vec3 vec) {
		return x * vec.x + y * vec.y + z * vec.z;
	}
	void rotate(Vec3* rotation) {
		float alpha = rotation->x;
		float beta = rotation->y;
		float gamma = rotation->z;

		float prev_x = x;
		float prev_y = y;
		float prev_z = z;
		
		if (gamma) { // Rotate around Z axis (gamma)
			x = prev_x * cos(gamma) - prev_y * sin(gamma);
			y = prev_x * sin(gamma) + prev_y * cos(gamma);
			// z = prev_z;
		}

		if (beta) { // Rotate around Y axis (beta)
			prev_x = x;
			prev_y = y;
			prev_z = z;
			x =  prev_x * cos(beta) + prev_z * sin(beta);
			// y =  prev_y;
			z = -prev_x * sin(beta) + prev_z * cos(beta);
		}

		if (alpha) { // Rotate around X axis (alpha)
			prev_x = x;
			prev_y = y;
			prev_z = z;
			// x =  prev_x;
			y =  prev_y * cos(alpha) - prev_z * sin(alpha);
			z =  prev_y * sin(alpha) + prev_z * cos(alpha);
		}
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
	Vec2 upper_bound, lower_bound;
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
};

struct Triangle {
	Vec3 vertices[3];
	RGBAColor col;

	Triangle2d flatten() {
		Triangle2d tri = {{
			vertices[0].flatten(),
			vertices[1].flatten(),
			vertices[2].flatten(),
		},
		col
		};
		return tri;
	}
};