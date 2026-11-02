#pragma once

#include <iostream>

#include "geometry.h"
#include "stb_image.h" // NOTE: STB_IMAGE_IMPLEMENTATION defined in objects - needed for compilation

struct Camera
{
    RGBAColor bg_color;
    bool is_skybox_loaded = false;

    float field_of_view_x;
    float field_of_view_y;

    Vec3 position = {0,0,100};
	Vec3 rotation = {0,0,0};

    PixelArr skybox_tex_pixels;
    int skybox_tex_width, skybox_tex_height, skybox_tex_channels;

    // Camera(RGBAColor col, const int width, const int height) {
    //     bg_color = col;
    //     set_fov_from_degrees(90, height / (float)width);
    // }
    Camera() {
        bg_color = {200,200,255};
        // bg_color = {255,0,0};
    }

    ~Camera() {
        if (is_skybox_loaded) stbi_image_free(skybox_tex_pixels);
    }

    void set_fov_from_degrees(float value, float aspect_ratio) {
        field_of_view_x = degrees2radians(value);
        field_of_view_y = 2.0f * atanf(tanf(field_of_view_x/2) * aspect_ratio);
    }

    inline void rotate(Vec3 delta) {if (delta.z != 0) throw std::runtime_error("Camera may not turn around Z achsis");rotation = rotation + delta;}
	void rotate_degrees(Vec3 delta_degrees) {
        Vec3 delta = {
            degrees2radians(delta_degrees.x),
            degrees2radians(delta_degrees.y),
            degrees2radians(delta_degrees.z),
        };
        rotation = rotation + delta;
    }
    inline void rotate_degrees_x(float value) {rotation.x = degrees2radians(value) + rotation.x;}
    inline void rotate_degrees_y(float value) {rotation.y = degrees2radians(value) + rotation.y;}
    inline void rotate_degrees_z(float value) {rotation.z = degrees2radians(value) + rotation.z;}

    inline void move(Vec3 delta) {position = position + delta;}
    inline void move_x(float value) {position.x = value + position.x;}
    inline void move_y(float value) {position.y = value + position.y;}
    inline void move_z(float value) {position.z = value + position.z;}

    void solid_color(RGBAColor col) {
        bg_color = col;
    } 

    void skybox_from_png(const char* filename) {
        skybox_tex_pixels = stbi_load(filename, &skybox_tex_width, &skybox_tex_height, &skybox_tex_channels, 4);

        if (!skybox_tex_pixels) throw std::runtime_error("Failed to load image");
        if (skybox_tex_channels != 4 && skybox_tex_channels != 3) throw std::runtime_error("Image does not support RGB (RGBA)");

        is_skybox_loaded = true;
        std::cout << "Loaded " << filename << ": " << skybox_tex_width << "x" << skybox_tex_height << " with " << skybox_tex_channels << " skybox_tex_channels\n";
    }

    RGBAColor color_at(int x, int y) {
        int i = (mod(y, skybox_tex_height) * skybox_tex_width + mod(x, skybox_tex_width)) * 4;
        return {
            skybox_tex_pixels[i+0],
            skybox_tex_pixels[i+1],
            skybox_tex_pixels[i+2],
            255,
        };
    }

    RGBAColor get_bg_at(float relative_screen_x, float relative_screen_y) {
        if (!is_skybox_loaded) return bg_color;

        // Von Screenspace -> Richtung im Kameraraum
        float x_cam = relative_screen_x * tanf(field_of_view_x/2);
        float y_cam = relative_screen_y * tanf(field_of_view_y/2);
        float z_cam = -1.0f; // Kamera schaut nach -Z

        Vec3 dir = {x_cam, y_cam, z_cam};

        // normalize
        dir = dir / dir.magnitude();

        // Rotation der Kamera anwenden
        dir.rotate_order_reversed(rotation);

        // Richtung -> sphärische Koordinaten
        float alpha = atan2(dir.x, dir.z);   // horizontal [-π, π]
        float beta  = asin(dir.y);   // vertikal  [-π/2, π/2]

        // Normiert auf Texturkoordinaten
        int x = alpha / (2.0f * M_PI) * skybox_tex_width;
        int y = (M_PI/2 - beta) / M_PI * skybox_tex_height;

        #ifdef DRAW_SKYBOX_GRID
        RGBAColor col = {0,0,0};
        if ((int)(dir.x*50) % 10 == 0) col.g = 255;
        if ((int)(dir.y*50) % 10 == 0) col.r = 255;
        return col;
        #endif
        return color_at(x, y);
    }
};

