
#include "scene.h"

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "rasterizer.h"

RGBAColor random_color() {
	return RGBAColor{
		static_cast<unsigned char>(rand() % 256),
		static_cast<unsigned char>(rand() % 256),
		static_cast<unsigned char>(rand() % 256),
		255 // full opacity
	};
}

void d3Object::transform_position(Vec3 delta_pos) {
	position = position + delta_pos;
}
void d3Object::transform_rotation(Vec3 delta_rot) {
	rotation = rotation + delta_rot;
}
void d3Object::transform_rotation_degrees(Vec3 delta_rot_degrees) {
	Vec3 delta_rot = {
		degrees2radians(delta_rot_degrees.x),
		degrees2radians(delta_rot_degrees.y),
		degrees2radians(delta_rot_degrees.z),
	};
	transform_rotation(delta_rot);
}
void d3Object::transform_scale(Vec3 delta_sca) {
	scale = scale + delta_sca;
}
void d3Object::transform_scale(float delta_sca) {
	scale = scale * delta_sca;
}

void d3Object::apply_transformation_in_array(std::vector<Triangle>* global_triangles) {
	for (int i = 0; i < triangles.size(); i++)
	{
		// copy triangle
		global_triangles->emplace_back(triangles[i]); // constructs in-place inside the vector
    	Triangle* transformed_tri = &global_triangles->back();

		// adjust verticies
		for (int j = 0; j < 3; j++) {
			// scale
			transformed_tri->vertices[j] = transformed_tri->vertices[j] * scale;
			// rotation
			transformed_tri->vertices[j].rotate(&rotation);
			// position
			transformed_tri->vertices[j] = transformed_tri->vertices[j] + position;
		}
	}
}


d3Object d3Object::load(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open OBJ file: " << filename << std::endl;
		return {};
	}

	std::vector<Vec3> vertices;
	std::vector<Triangle> triangles;

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);

		if (line.rfind("v ", 0) == 0) { // vertex line
			Vec3 v;
			iss.ignore(2); // skip "v "
			iss >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		} else if (line.rfind("f ", 0) == 0) { // face line
			std::vector<int> indices;
			iss.ignore(2); // skip "f "

			std::string token;
			while (iss >> token) {
				// Parse only the first number before any slashes
				std::istringstream tokenStream(token);
				std::string indexStr;
				std::getline(tokenStream, indexStr, '/'); // get the v index before '/'
				int index = std::stoi(indexStr);
				indices.push_back(index - 1); // OBJ is 1-based
			}

			if (indices.size() >= 3) {
				// Triangulate (just first 3 vertices)
				Triangle tri = {
					vertices[indices[0]],
					vertices[indices[1]],
					vertices[indices[2]],
					random_color()
				};
				triangles.push_back(tri);
			}
		}
	}

	d3Object obj;
	obj.name = filename;
	obj.triangles = triangles;

	return obj;
}


d3Object* Scene::loadObj(const std::string& filename) {
    objects.emplace_back(filename); // constructs in-place inside the vector
    d3Object* obj_ptr = &objects.back();
    std::cout << "Loaded " << filename << " successfully! " << obj_ptr->triangles.size() << " triangles found\n";
    return obj_ptr;
}

void Scene::render(unsigned char* pixels, const int width, const int height) {
	// pack triangles
	std::vector<Triangle> triangles;

	for (int obj_index = 0; obj_index < objects.size(); obj_index++)
	{
		objects[obj_index].apply_transformation_in_array(&triangles);
	}

	orthographic_view(pixels, width, height, &triangles, bg_color);
	//draw_vertices(pixels, width, height, &triangles);
}

