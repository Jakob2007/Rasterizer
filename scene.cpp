
#include "scene.h"

#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <iostream>
#include <GLFW/glfw3.h>

#include "world.h"
#include "BVH.h"

#define NUM_THREADS 14

namespace projection_utils {
	const int NEAR_PLAIN = -1;

	inline float calc_projection_coefficient(float field_of_view, int width) {
		return width/tan(field_of_view/2);
	}

	inline bool is_tri_behind_cam(Triangle& tri) {
		// * Clipping at z = NEARPLAIN
		return (tri.vertices[0].z > NEAR_PLAIN && tri.vertices[1].z > NEAR_PLAIN && tri.vertices[2].z > NEAR_PLAIN);
	}

	inline bool is_triangle_facing_camera(Triangle& tri) {
		// ! is cause for flicering at the edge
		#ifdef DISABLE_BACKFACE_CULLING
			return true;
		#endif
		return tri.normal.dot(tri.get_center()) < 10; // threashhold chosen randomly weil funktioniert
	}

	inline Vec2 project_vector(Vec3 vec, float projection_coefficient) {
		// z clipped to max -1
		return {-vec.x / std::min(vec.z, -1.0f) * projection_coefficient, -vec.y / std::min(vec.z, -1.0f) * projection_coefficient};
	}

	Triangle2d project_triangle(Triangle &tri, float projection_coefficient) {
		Triangle2d projected_tri = {
			{
				project_vector(tri.vertices[0], projection_coefficient),
				project_vector(tri.vertices[1], projection_coefficient),
				project_vector(tri.vertices[2], projection_coefficient),
			},
			tri.col
		};
		return projected_tri;
	}

	inline bool is_tri_on_screen(Triangle2d* tri, const int half_width, const int half_height) { // hitbox based
		return !(
			tri->lower_bound.x > half_width ||
			tri->upper_bound.x < -half_width ||
			tri->lower_bound.y > half_height ||
			tri->upper_bound.y < -half_height
		);
	}

	void sort_triangles_by_center_dist(std::vector<Triangle2d>& vec) {
		// TODO use faster algorithem
		for (size_t i = 0; i < vec.size(); ++i) {
			for (size_t j = i + 1; j < vec.size(); ++j) {
				if (vec[j].square_center_dist < vec[i].square_center_dist) {
					std::swap(vec[i], vec[j]);
				}
			}
		}
	}

	inline float get_lighting_scalar(Vec3 normal, Vec3 source, float intensity) {
		return std::max((normal.dot(source)+1)/2, 0.0f) * intensity;
	}

	void draw_vertices(PixelArr pixels, const int width, const int height, std::vector<Triangle>& triangles) {
		for (int tri_count = 0; tri_count < triangles.size(); tri_count++)
		{
			Triangle tri = triangles[tri_count];
			// Vertex 1 - Red
			int x1 = tri.vertices[0].x + width/2;
			int y1 = tri.vertices[0].y + height/2;
			int i1 = (y1 * width + x1);
			RGBAColor::draw_rgb(pixels, i1, 255, 0, 0);

			// Vertex 2 - Green
			int x2 = tri.vertices[1].x + width/2;
			int y2 = tri.vertices[1].y + height/2;
			int i2 = (y2 * width + x2) * 4;
			RGBAColor::draw_rgb(pixels, i1, 0, 255, 0);

			// Vertex 3 - Blue
			int x3 = tri.vertices[2].x + width/2;
			int y3 = tri.vertices[2].y + height/2;
			int i3 = (y3 * width + x3) * 4;
			RGBAColor::draw_rgb(pixels, i1, 0, 0, 255);
		}
	}
}

// RGBAColor Scene::_get_lighting(Triangle& tri) { // flat
// 	// TODO: make light array with light_obj+intensity
// 	float scalar = projection_utils::get_lighting_scalar(tri.normal, global_light_direction, .5) + ambient_light;
// 	return tri.col * scalar;
// }

void Scene::_vertex_shader(d3Object& obj, std::vector<Triangle>& out) {
	// apply transformation
	std::vector<Vec3> transformed_vertices;
	std::vector<Vec3> transformed_vertices_normals;
	for (int i = 0; i < obj.vertices.size(); i++) {
		// scale
		Vec3 new_vertice = obj.vertices[i] * obj.scale;
		// rotation
		new_vertice.rotate(obj.rotation);
		// position
		new_vertice = new_vertice + obj.position;
		// camera position
		new_vertice = new_vertice - camera.position;
		// camera rotation
		new_vertice.rotate(camera.rotation*-1);
		
		transformed_vertices.push_back(new_vertice);

		// vertex normal
		Vec3 normal = obj.per_vertex_normals[i];
		normal.rotate(obj.rotation);
		transformed_vertices_normals.push_back(normal);
	}

	// create triangles
	for (int i = 0; i < obj.faces.size(); i++)
	{
		// rotate normal
		Vec3 transformed_normal = obj.normals[i];
		transformed_normal.rotate(obj.rotation);
		transformed_normal.rotate(camera.rotation*-1);

		out.emplace_back(
			Triangle{
				transformed_vertices[obj.faces[i][0]],
				transformed_vertices[obj.faces[i][1]],
				transformed_vertices[obj.faces[i][2]],
				transformed_normal,
				obj.color,
				transformed_vertices_normals[obj.faces[i][0]],
				transformed_vertices_normals[obj.faces[i][1]],
				transformed_vertices_normals[obj.faces[i][2]]
			}
		);
	}
}

void Scene::_optimize_triangles(std::vector<Triangle2d>& out, std::vector<Triangle>& triangles, const int width, const int height) {
	const float projection_coefficient = projection_utils::calc_projection_coefficient(camera.field_of_view_x, width);
	const float projection_coefficient_y = projection_utils::calc_projection_coefficient(camera.field_of_view_y, height);
	// missmatch in x and y fov?

	for (int tri_count = 0; tri_count < triangles.size(); tri_count++) {
		Triangle tri = triangles[tri_count];
		if (projection_utils::is_tri_behind_cam(tri)) continue;
		if (!projection_utils::is_triangle_facing_camera(tri)) continue;
		
		Triangle2d ptri = projection_utils::project_triangle(tri, projection_coefficient);
		
		ptri.calc_bounding_box();
		if (!projection_utils::is_tri_on_screen(&ptri, width / 2, height / 2)) continue;

		ptri.square_center_dist = tri.get_center().square_magnitude();

		#ifdef DRAW_NORMAL_COLOR
		ptri.col = ((tri.normal + Vec3({1,1,1})) * 0.5f).to_color(); // * draw normals
		#endif
		ptri.per_vertex_normals[0] = tri.per_vertex_normals[0];
		ptri.per_vertex_normals[1] = tri.per_vertex_normals[1];
		ptri.per_vertex_normals[2] = tri.per_vertex_normals[2];
		out.emplace_back(ptri);
	}
}

RGBAColor gouraud_shading(std::vector<Light>& lights, Vec2& point, Triangle2d* target) {
	RGBAColor color = {0,0,0};
	for (int i = 0; i < lights.size(); i++)
	{
		Vec3 b_coords = target->barycentric_coordinates(point);
		float scalar_a = lights[i].get_scalar(target->per_vertex_normals[0]) * b_coords[0];
		float scalar_b = lights[i].get_scalar(target->per_vertex_normals[1]) * b_coords[1];
		float scalar_c = lights[i].get_scalar(target->per_vertex_normals[2]) * b_coords[2];
		color = color + target->col * (scalar_a + scalar_b + scalar_c) * lights[i].intensity;
	}
	color = color / lights.size();
	return color;
}
RGBAColor phong_shading(std::vector<Light>& lights, Vec2& point, Triangle2d* target) {
	RGBAColor color = {0,0,0};
	for (int i = 0; i < lights.size(); i++)
	{
		Vec3 b_coords = target->barycentric_coordinates(point);
		Vec3 scaled_normal_a = target->per_vertex_normals[0] * b_coords[0];
		Vec3 scaled_normal_b = target->per_vertex_normals[1] * b_coords[1];
		Vec3 scaled_normal_c = target->per_vertex_normals[2] * b_coords[2];
		float scalar = lights[i].get_scalar((scaled_normal_a + scaled_normal_b + scaled_normal_c) / 3);
		color = color + target->col * scalar * lights[i].intensity;
	}
	color = color / lights.size();
	return color;
}

RGBAColor Scene::_fragment_shader(int i) {
	int width = world->width;
	int height = world->height;
	Vec2 point = {(float)(i % width - width/2), (float)(i / width - height/2)};
	Triangle2d* target = BVH->treverse(point);
	if (target == nullptr) {
		return camera.get_bg_at(point.x / (float)width, point.y / (float)height);
	}
	#ifdef DRAW_BARYCENTRIC_COORDS
		return target->barycentric_coordinates(point).to_color();
	#endif
	
	// RGBAColor color = gouraud_shading(lights, point, target);
	RGBAColor color = phong_shading(lights, point, target);

	return color;
}

void Scene::_rasterize_chunk(int begin, int chunk_size) {
	// loop over each pixel
	for (int i = begin; i < begin+chunk_size; i++) {
		_fragment_shader(i).draw_this(world->pixels, i);
	}
}

void Scene::_parallel_rasterization() {
	std::thread threads[NUM_THREADS];
	int pixel_count = world->width * world->height;
	const int chunk_size = pixel_count / NUM_THREADS;

	for (int i = 0; i < NUM_THREADS; i++) {
		int begin = i*chunk_size;
		int size = (i == NUM_THREADS - 1) ? (pixel_count - begin) : chunk_size;
		threads[i] = std::thread([this, begin, size]() {
			_rasterize_chunk(begin, size);
		});
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		threads[i].join();
	}
}

void Scene::render(PixelArr pixels, const int width, const int height) {
	// pack triangles
	std::vector<Triangle> triangles;
	for (int obj_index = 0; obj_index < objects.size(); obj_index++)
	{
		_vertex_shader(objects[obj_index], triangles);
	}

	std::vector<Triangle2d> projected_triangles;
	_optimize_triangles(projected_triangles, triangles, width, height);

	BVH = BoundingVolumeHierarchy::empty_BVH();
	BVH->construct(projected_triangles);

	// parallel_rasterization_from_vector(width, height, bg_color, pixels, projected_triangles);
	_parallel_rasterization();
	// draw_vertices(pixels, width, height, triangles);
	#ifdef DRAW_BVH
		BVH->draw(pixels, width, height);
	#endif
}

d3Object* Scene::empty_object() {
    objects.emplace_back(); // constructs in-place inside the vector
    d3Object* obj_ptr = &objects.back();
    return obj_ptr;
}
d3Object* Scene::load_object(const std::string& filename) {
    objects.emplace_back(filename); // constructs in-place inside the vector
    d3Object* obj_ptr = &objects.back();
    std::cout << "Loaded " << filename << " successfully! " << obj_ptr->faces.size() << " triangles found\n";
    return obj_ptr;
}

void Scene::quit() {world->running = false;}

d3Object* Scene::find_by_index(int i) {
    d3Object* obj_ptr = &objects[i];
    return obj_ptr;
}

// d3Object* Scene::find_by_tag(std::string_view tag) { // TODO: implement Tags
//     d3Object* obj_ptr = &objects[0];
//     return obj_ptr;
// }

std::vector<d3Object*> Scene::find_by_name(const std::string& filename) {
	std::vector<d3Object*> result;
	for (auto& obj : objects) {
		if (obj.name == filename) {
			result.push_back(&obj);
		}
	}
	return result;
}

// void Scene::set_ambient_light(float value) {
// 	ambient_light = value;
// }

int Scene::is_key_pressed(int key) {
	return glfwGetKey(world->window, key) == GLFW_PRESS;
}

Vec2 Scene::get_mouse_position() {
	double xpos, ypos;
	glfwGetCursorPos(world->window, &xpos, &ypos);
	return {(float)xpos, (float)ypos};
}
void Scene::set_mouse_visability(bool is_visable) {
	if (!is_visable) glfwSetInputMode(world->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else glfwSetInputMode(world->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

