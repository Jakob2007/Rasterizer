
#include <fstream>
#include <sstream>
#include <iostream>

#include "objects.h"

#define STB_IMAGE_IMPLEMENTATION // must be included in at least one .cpp to compile correctly
#include "stb_image.h"

d3Object* d3Object::transform_position(Vec3 delta_pos) {
	position = position + delta_pos;
	return this;
}
d3Object* d3Object::transform_rotation(Vec3 delta_rot) {
	rotation = rotation + delta_rot;
	return this;
}
d3Object* d3Object::transform_rotation_degrees(Vec3 delta_rot_degrees) {
	Vec3 delta_rot = {
		degrees2radians(delta_rot_degrees.x),
		degrees2radians(delta_rot_degrees.y),
		degrees2radians(delta_rot_degrees.z),
	};
	transform_rotation(delta_rot);
	return this;
}
d3Object* d3Object::transform_scale(Vec3 delta_sca) {
	scale = scale + delta_sca;
	return this;
}
d3Object* d3Object::transform_scale(float delta_sca) {
	scale = scale * delta_sca;
	return this;
}

d3Object* d3Object::add_vertex(Vec3 vertex) {
	vertices.push_back(vertex);
	return this;
}
d3Object* d3Object::add_face(std::array<int, 3> face) {
	assert((face[0] <= vertices.size()));
	assert((face[1] <= vertices.size()));
	assert((face[2] <= vertices.size()));

	// Vec3 ab = vertices[face[1]] - vertices[face[0]];
	// Vec3 ac = vertices[face[2]] - vertices[face[0]];
	// Vec3 normal = ab.cross(ac);
	// normals.push_back(normal);

	faces.push_back(face);
	return this;
}

d3Object* d3Object::set_name(std::string new_name) {
	name = new_name;
	return this;
}

// TODO: combine in material stuct \/\/
d3Object* d3Object::set_color(RGBAColor new_color) {
    color = new_color;
    return this;
}
d3Object* d3Object::set_reflective_index(float value) {
	// TODO: implement reflectve value for triangles
	reflective_index = value;
	return this;
}

void d3Object::compute_per_vertex_normals() {
	per_vertex_normals.clear();
	std::vector<std::vector<Vec3>> face_normals_per_vertex;
	face_normals_per_vertex.resize(vertices.size());
	for (int i = 0; i < faces.size(); i++)
	{
		Vec3 normal = normals[i];
		// TODO: try weighted approach
		face_normals_per_vertex[faces[i][0]].push_back(normal);
		face_normals_per_vertex[faces[i][1]].push_back(normal);
		face_normals_per_vertex[faces[i][2]].push_back(normal);
	}
	for (int i = 0; i < vertices.size(); i++)
	{
		int count = face_normals_per_vertex[i].size();
		Vec3 result = {0,0,0};
		for (int j = 0; j < count; j++)
		{
			result = result + face_normals_per_vertex[i][j];
		}
		per_vertex_normals.push_back(result / count);
	}
}

void d3Object::refresh() {
	normals.clear();
	for (int i = 0; i < faces.size(); i++)
	{
		std::array<int, 3>& face = faces[i];
		Vec3 ab = vertices[face[1]] - vertices[face[0]];
		Vec3 ac = vertices[face[2]] - vertices[face[0]];
		Vec3 normal = ab.cross(ac);
		normal = normal / normal.magnitude();
		normals.push_back(normal);
		// normals.push_back({0,1,0});
	}

	compute_per_vertex_normals();
}

void d3Object::load(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open OBJ file: " << filename << std::endl;
	}

	name = filename;
	color = RGBAColor::random();

	std::vector<Vec3> included_normals;

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);

		if (line.rfind("v ", 0) == 0) { // vertex line
			Vec3 v;
			iss.ignore(2); // skip "v "
			iss >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		} else if (line.rfind("vn ", 0) == 0) { // normal line
			Vec3 n;
			iss.ignore(3); // skip "vn "
			iss >> n.x >> n.y >> n.z;
			included_normals.push_back(n);
		} else if (line.rfind("f ", 0) == 0) { // face line
			std::vector<int> indices;
			iss.ignore(2); // skip "f "

			std::string token;
			int normal_index = -1;
			bool normal_found = false;
			while (iss >> token) {
				// Parse the vertex and normal index (format: v/vt/vn or v//vn or v)
				std::istringstream tokenStream(token);
				std::string vStr, vtStr, vnStr;
				std::getline(tokenStream, vStr, '/');
				std::getline(tokenStream, vtStr, '/');
				std::getline(tokenStream, vnStr, '/');
				int v_idx = std::stoi(vStr);
				indices.push_back(v_idx - 1); // OBJ is 1-based

				if (!vnStr.empty() && !normal_found) {
					normal_index = std::stoi(vnStr) - 1;
					normal_found = true;
				}
			}

			if (indices.size() >= 3) {
				// Triangulate (just first 3 vertices)
				faces.push_back({indices[0], indices[1], indices[2]});
				if (normal_found && normal_index >= 0 && normal_index < included_normals.size()) {
					normals.push_back(included_normals[normal_index]);
				} else {
					Vec3 vec1 = vertices[indices[0]];
					Vec3 vec2 = vertices[indices[1]];
					Vec3 vec3 = vertices[indices[2]];
					Vec3 normal = (vec2 - vec1).cross((vec3 - vec1));
					normal = normal / normal.magnitude();
					normals.push_back(normal);
				}
			}
		}
	}

	compute_per_vertex_normals();
}