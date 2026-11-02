#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "geometry.h"

struct d3Object {
	std::vector<Triangle> triangles;
	std::string name;
	
	bool visable = true;
	Vec3 position = {0,0,0};
	Vec3 rotation = {0,0,0}; // radians
	Vec3 scale = {1,1,1};

	// Constructor
	d3Object() {};
	d3Object(const std::string& filename) {
		*this = load(filename);
	}

	d3Object load(const std::string& filename);

	void transform_position(Vec3 delta_pos);
	void transform_rotation(Vec3 delta_rot);
	void transform_rotation_degrees(Vec3 delta_rot);
	void transform_scale(Vec3 delta_sca);
	void transform_scale(float delta_sca);

	void apply_transformation_in_array(std::vector<Triangle>* global_triangles);
};

class Scene {
public:
	RGBAColor bg_color = {0,0,0};
    std::vector<d3Object> objects;

    virtual void setup() = 0;               // Pure virtual
    virtual void update(float delta_time) = 0;

    d3Object* loadObj(const std::string& filename); // Already implemented
	void render(unsigned char* pixels, const int width, const int height);

    virtual ~Scene() {}
};




