#pragma once

#include "geometry.h"

struct d3Object {
	std::string name;
	std::vector<Vec3> vertices;
	std::vector<Vec3> per_vertex_normals;
	std::vector<std::array<int, 3>> faces;
	std::vector<Vec3> normals;

	RGBAColor color = {200,200,200,255};
	float reflective_index = 1;
	
	bool visable = true;
	Vec3 position = {0,0,0};
	Vec3 rotation = {0,0,0}; // radians
	Vec3 scale = {1,1,1};

	// Constructor
	d3Object() {};
	d3Object(const std::string& filename) {
		load(filename);
	}
	d3Object copy__() const {
		d3Object obj;
		obj.name = name;
		obj.vertices = vertices;
		obj.normals = normals;
		obj.faces = faces;
		obj.color = color;
		obj.visable = visable;
		obj.position = position;
		obj.rotation = rotation;
		obj.scale = scale;
		return obj;
	}

	void compute_per_vertex_normals();
	void refresh();
	void load(const std::string& filename);

	d3Object* transform_position(Vec3 delta_pos);
	d3Object* transform_rotation(Vec3 delta_rot);
	d3Object* transform_rotation_degrees(Vec3 delta_rot);
	d3Object* transform_scale(Vec3 delta_sca);
	d3Object* transform_scale(float delta_sca);
	d3Object* add_vertex(Vec3 vertex);
	d3Object* add_face(std::array<int, 3> face);
	d3Object* set_name(std::string name);
	d3Object* set_color(RGBAColor new_color);
	d3Object* set_reflective_index(float value); // var not used
};