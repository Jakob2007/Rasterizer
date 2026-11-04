#pragma once

#include <vector>
#include <string>

#include "geometry.h"
#include "objects.h"
#include "camera.h"
#include "BVH.h"

// Forward declare
struct World;

struct Light {
	Vec3 direction = {0,-1,0};
	RGBAColor col = {255,255,255};
	float intensity = 1.5;

	float get_scalar(Vec3 normal) {
		return std::max((direction.dot(normal)+1.0f)/2.0f, 0.0f);
	}
};

class Scene {
private:
	void _vertex_shader(d3Object& obj, std::vector<Triangle>& out);
	void _optimize_triangles(std::vector<Triangle2d>& out, std::vector<Triangle>& triangles, const int width, const int height);
	RGBAColor _fragment_shader(int i);
	void _rasterize_chunk(int begin, int chunk_size);
	void _parallel_rasterization();
public:
	// TODO: make private
	RGBAColor bg_color = {0,0,0};
    std::vector<d3Object> objects;
	std::vector<Light> lights;

	float ambient_light = .5;

	bool is_static = false;
	std::unique_ptr<BoundingVolumeHierarchy> BVH;
	Camera camera;
	World* world;
public:
	// TODO: inline stuff // ? why
    virtual void setup() = 0;
    virtual void update(float delta_time) = 0;

	void render(PixelArr pixels, const int width, const int height);
	void quit();
    
	d3Object* empty_object();
	d3Object* load_object(const std::string& filename);
	std::vector<d3Object*> find_by_name(const std::string& filename);
	d3Object* find_by_index(int i);

	void set_ambient_light(RGBAColor col);
	inline void add_light(Vec3 dir, RGBAColor col) {lights.push_back({dir / dir.magnitude(), col});}
	inline void add_light(Vec3 dir, unsigned char intensity) {lights.push_back({dir / dir.magnitude(), RGBAColor{intensity,intensity,intensity,255}});}

	int is_key_pressed(int key);
	Vec2 get_mouse_position();
	void set_mouse_visability(bool is_visable);

    virtual ~Scene() {};
};




