#pragma once

#include <vector>

#include "geometry.h"

void draw_vertices(unsigned char* pixels, const int width, const int height, std::vector<Triangle>* triangles);
void orthographic_view(unsigned char* pixels, const int width, const int height, std::vector<Triangle>* triangles, RGBAColor bg_color);