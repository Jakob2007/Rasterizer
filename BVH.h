#pragma once

#include <vector>

#include "geometry.h"

#define DEAFULT_DEPTH 10

class BoundingVolumeHierarchy {
private:
    std::unique_ptr<BoundingVolumeHierarchy> _child_a;
    std::unique_ptr<BoundingVolumeHierarchy> _child_b;
    Vec2 _upper_bound = {-INFINITY, -INFINITY};
    Vec2 _lower_bound = {INFINITY, INFINITY};
	RGBAColor _col = RGBAColor::random();
    std::vector<Triangle2d> _triangles;
	bool _is_leaf_node;
private:
	void _find_bounds(std::vector<Triangle2d>& triangles);

	void _sort_by_axis(std::vector<Triangle2d>& triangles, const char axis);

	void _split_by_longer_axis(std::vector<Triangle2d>& triangles, std::vector<Triangle2d>& out_a, std::vector<Triangle2d>& out_b);

    bool _point_in_bounding_box(Vec2& point);
public:
    BoundingVolumeHierarchy() {};

    static std::unique_ptr<BoundingVolumeHierarchy> empty_BVH();

    void construct(std::vector<Triangle2d>& triangles, const int depth=DEAFULT_DEPTH);

	float construct_timed(std::vector<Triangle2d> triangles, const int depth=2, bool log=false);

	Triangle2d* treverse(Vec2& point);

	void draw(PixelArr pixels, const int width, const int height);
};