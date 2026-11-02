
#include <vector>

#include "geometry.h"
#include "BVH.h"

void BoundingVolumeHierarchy::_find_bounds(std::vector<Triangle2d>& triangles) {
    for (size_t i = 0; i < triangles.size(); i++)
    {
        Triangle2d tri = triangles[i];
        _lower_bound.x = std::min(tri.lower_bound.x, _lower_bound.x);
        _lower_bound.y = std::min(tri.lower_bound.y, _lower_bound.y);
        _upper_bound.x = std::max(tri.upper_bound.x, _upper_bound.x);
        _upper_bound.y = std::max(tri.upper_bound.y, _upper_bound.y);
    }
}

void BoundingVolumeHierarchy::_split_by_longer_axis(std::vector<Triangle2d>& triangles, std::vector<Triangle2d>& out_a, std::vector<Triangle2d>& out_b) {
    std::function<float(const Vec2&)> get_axis_coord;
    if (_upper_bound.x-_lower_bound.x > _upper_bound.y-_lower_bound.y) get_axis_coord = [](const Vec2& vec) { return vec.x; };
    else get_axis_coord = [](const Vec2& vec) { return vec.y; };
    
    float threashhold = get_axis_coord(_lower_bound) + (get_axis_coord(_upper_bound) - get_axis_coord(_lower_bound)) / 2;
    
    for (size_t i = 0; i < triangles.size(); i++)
    {
        if(get_axis_coord(triangles[i].lower_bound) < threashhold) {
            out_a.push_back(triangles[i]);
        } else {
            out_b.push_back(triangles[i]);
        }
    }
}

inline bool BoundingVolumeHierarchy::_point_in_bounding_box(Vec2& point) {
    return (point.x >= _lower_bound.x && point.x <= _upper_bound.x &&
            point.y >= _lower_bound.y && point.y <= _upper_bound.y);
}

std::unique_ptr<BoundingVolumeHierarchy> BoundingVolumeHierarchy::empty_BVH() {
    // return std::unique_ptr<BoundingVolumeHierarchy>(new BoundingVolumeHierarchy());
    return std::make_unique<BoundingVolumeHierarchy>();
}

void BoundingVolumeHierarchy::construct(std::vector<Triangle2d>& triangles, const int depth) {
    _find_bounds(triangles);

    _is_leaf_node = depth <= 0 || triangles.size() <= 1;
    if (_is_leaf_node) {
        _triangles = triangles;
        return;
    }

    std::vector<Triangle2d> child_a_vec, child_b_vec;
    _split_by_longer_axis(triangles, child_a_vec, child_b_vec);

    _child_a = BoundingVolumeHierarchy::empty_BVH();
    _child_a->construct(child_a_vec, depth-1);

    _child_b = BoundingVolumeHierarchy::empty_BVH();
    _child_b->construct(child_b_vec, depth-1);
}

float BoundingVolumeHierarchy::construct_timed(std::vector<Triangle2d> triangles, const int depth, bool log) {
    const auto& start = std::chrono::high_resolution_clock::now();
    construct(triangles, depth);
    const auto& stop = std::chrono::high_resolution_clock::now();
    auto render_duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    if (log) std::cout << "BVH construction time: " << render_duration << " depth: " << depth << std::endl;
    return render_duration;
}

Triangle2d* BoundingVolumeHierarchy::treverse(Vec2& point) { // depth estimate
    if (!_point_in_bounding_box(point)) return nullptr;
    if (_is_leaf_node) {
        if (_triangles.size() == 0) return nullptr;
        float min_dist = INFINITY;
        Triangle2d* tri = nullptr;
        for (size_t i = 0; i < _triangles.size(); i++)
        {
            Triangle2d& ctri = _triangles[i];
            if (!ctri.point_in_bounding_box(point)) continue;
            #ifdef DRAW_BOUNDING_BOX
                min_dist = ctri.square_center_dist; tri = &ctri; continue; // early break
            #endif
            if (!ctri.did_point_hit(point)) continue;
            if (ctri.square_center_dist >= min_dist) continue; // * approximate dist
            // ctri.col = _col; // * debug
            min_dist = ctri.square_center_dist;
            tri = &ctri;
        }
        return tri;
    }
    Triangle2d* tri_a = _child_a->treverse(point);
    Triangle2d* tri_b = _child_b->treverse(point);

    if (tri_a == nullptr && tri_b == nullptr) return nullptr;
    if (tri_a == nullptr) return tri_b;
    if (tri_b == nullptr) return tri_a;
    if (tri_a->square_center_dist > tri_b->square_center_dist) return tri_b;
    return tri_a;
}

void BoundingVolumeHierarchy::draw(PixelArr pixels, const int width, const int height) {
    std::srand((unsigned int)_upper_bound.x);
    int x1 = std::max((int)(_lower_bound.x + width/2), 0);
    int x2 = std::min((int)(_upper_bound.x + width/2), width-1);
    int y1 = std::max((int)(_lower_bound.y + height/2), 0);
    int y2 = std::min((int)(_upper_bound.y + height/2), height-1);

    for (int x = x1; x <= x2; ++x) {
        int i1 = y1 * width + x;
        int i2 = y2 * width + x;
        _col.draw_this(pixels, i1);
        _col.draw_this(pixels, i2);
    }

    for (int y = y1; y <= y2; ++y) {
        int i1 = y * width + x1;
        int i2 = y * width + x2;
        _col.draw_this(pixels, i1);
        _col.draw_this(pixels, i2);
    }
    if (!_is_leaf_node) {
        _child_a->draw(pixels, width, height);
        _child_b->draw(pixels, width, height);
    }
}