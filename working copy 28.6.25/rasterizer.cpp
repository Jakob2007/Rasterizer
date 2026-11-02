
#include "rasterizer.h"
#include "geometry.h"

#include <iostream>
using namespace std;

void render_gradient(unsigned char* pixels, const int width, const int height) {
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int i = (y * width + x) * 4;
			pixels[i + 0] = (unsigned char)(x * 255 / width);  // Red gradient
			pixels[i + 1] = (unsigned char)(y * 255 / height); // Green gradient
			pixels[i + 2] = 128;                               // Blue constant
			pixels[i + 3] = 255;                               // Alpha = fully opaque
		}
	}
}

bool vec_is_right_side(Vec2 relative_point_vec, Vec2 edge) {
	// kreuzprodukt - vector rechter winkel - normalenvektor
	Vec2 normal = {-edge.y, edge.x};

	// skalarprodukt
	int scalar = normal.dot(relative_point_vec);

	return scalar < 0;
}

bool is_triangle_flat(Triangle2d* tri) {
	float area2 = (tri->vertices[1].x - tri->vertices[0].x)*(tri->vertices[2].y - tri->vertices[0].y) - (tri->vertices[1].y - tri->vertices[0].y)*(tri->vertices[2].x - tri->vertices[0].x);
	return std::abs(area2) < 1e-6; // Use a tolerance for floating point comparison
}

bool is_in_bounds(Vec2 point, Triangle2d* tri) {
	return (point.x >= tri->lower_bound.x && point.x <= tri->upper_bound.x &&
			point.y >= tri->lower_bound.y && point.y <= tri->upper_bound.y);
}

bool d2_triangle_hit(Vec2 point, Triangle2d* tri) {
	for (int i=0; i<3; i++) {
		// finde edge
		Vec2 p1 = tri->vertices[i];
		Vec2 p2 = tri->vertices[(i+1) % 3];
		Vec2 edge = p2 - p1;

		// find conecting vector
		Vec2 relative_point_vec = point - p1;

		// loop edges
		if (vec_is_right_side(relative_point_vec, edge)) {
			return false;
		}
	}
	return true;
}

void draw_vertices(unsigned char* pixels, const int width, const int height, std::vector<Triangle>* triangles) {
	for (int tri_count = 0; tri_count < triangles->size(); tri_count++)
	{
		Triangle tri = (*triangles)[tri_count];
		// Vertex 1 - Red
		int x1 = tri.vertices[0].x + width/2;
		int y1 = tri.vertices[0].y + height/2;
		int i1 = (y1 * width + x1) * 4;
		pixels[i1 + 0] = 255;
		pixels[i1 + 1] = 0;
		pixels[i1 + 2] = 0;
		pixels[i1 + 3] = 255;

		// Vertex 2 - Green
		int x2 = tri.vertices[1].x + width/2;
		int y2 = tri.vertices[1].y + height/2;
		int i2 = (y2 * width + x2) * 4;
		pixels[i2 + 0] = 0;
		pixels[i2 + 1] = 255;
		pixels[i2 + 2] = 0;
		pixels[i2 + 3] = 255;

		// Vertex 3 - Blue
		int x3 = tri.vertices[2].x + width/2;
		int y3 = tri.vertices[2].y + height/2;
		int i3 = (y3 * width + x3) * 4;
		pixels[i3 + 0] = 0;
		pixels[i3 + 1] = 0;
		pixels[i3 + 2] = 255;
		pixels[i3 + 3] = 255;
	}
}

void orthographic_view(unsigned char* pixels, const int width, const int height, std::vector<Triangle>* triangles, RGBAColor bg_color) {
	std::vector<Triangle2d> projected_triangles;
	for (int tri_count = 0; tri_count < triangles->size(); tri_count++) {
		Triangle2d tri = (*triangles)[tri_count].flatten();
		
		if (is_triangle_flat(&tri)) {
			continue;
		}
		tri.calc_bounding_box();
		projected_triangles.emplace_back(tri);
	}
	// loop over each pixel
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 4;

			pixels[i + 0] = bg_color.r;
			pixels[i + 1] = bg_color.g;
			pixels[i + 2] = bg_color.b;
			pixels[i + 3] = bg_color.a;

			Vec2 point = {(float)(x - width/2), (float)(y - height/2)};

			// loop all triangles in scene
			for (int tri_count = 0; tri_count < projected_triangles.size(); tri_count++)
			{
				// TODO: check bounding box

				// check if triangle is hit
				Triangle2d* projected_triangle = &projected_triangles[tri_count];

				if (!is_in_bounds(point, projected_triangle)) {
					continue;
				}
				if (!d2_triangle_hit(point, projected_triangle)) {
					continue;
				}
				pixels[i + 0] = projected_triangle->col.r;
				pixels[i + 1] = projected_triangle->col.g;
				pixels[i + 2] = projected_triangle->col.b;
				pixels[i + 3] = projected_triangle->col.a;
			}
		}
	}
}

// void render(unsigned char* pixels, const int width, const int height) {
// 	// render_gradient(pixels, width, height);

// 	// load object
// 	d3Object object = loadOBJ("./data.obj");
// 	cout << "Loaded file successfully! " << object.triangles.size() << " triangles found\n"; 
	
// 	// start timer
// 	const auto& start = std::chrono::high_resolution_clock::now();

// 	// transform object
// 	object.rotate_axis(2, 45);
// 	object.transform_scale(100);

// 	float radians = 90 * M_PI / 180;
// 	Vec3 normal = {sin(radians), cos(radians), 0};
// 	Vec3 new_verticie = normal * 2;
// 	new_verticie.dump();
// 	float length = sqrt(new_verticie.x*new_verticie.x + new_verticie.y*new_verticie.y + new_verticie.z*new_verticie.z);
// 	cout << length << endl;

// 	object.triangles[0].vertices[0].dump();
// 	object.triangles[0].vertices[1].dump();
// 	object.triangles[0].vertices[2].dump();

// 	// render triangles
// 	orthographic_view(pixels, width, height, object.triangles);

// 	// end timer
// 	const auto& stop = std::chrono::high_resolution_clock::now();
// 	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
// 	cout << "execution time: " << duration << " ms" << endl;

// 	// sleep
// }

