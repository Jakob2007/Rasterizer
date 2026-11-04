#pragma once

#include <chrono>
#include <memory>
#include <math.h>

#include "scene_design.h"

struct World {
    
    static const int width = 800;
    static const int height = 800;
    const char* title = "Rasterizer";
    
    static const int scene_count = 2;
    static const int start_scene_index = START_SCENE_INDEX;
    const std::unique_ptr<Scene> scenes[scene_count] = {
        std::make_unique<MainScene>(),
        std::make_unique<Monkey>()
    };

    Scene* active_scene = nullptr;
    bool running = true;

    PixelArr pixels;
    GLuint texture;
    GLFWwindow* window;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_update;
    float current_delta_time = 0;

    int idx = 0;
    static const int fps_smoothing = 20; // purly cosmetic
    int durations[fps_smoothing] = {0};

    World() {}

    void setup();

    void terminate();

    // static void World::manage_input(GLFWwindow* win, int key, int scancode, int action, int mods);

    void update_scene();

    float set_delta_time_and_get_fps();

    void window_init();

    void texture_init(GLuint* texture);

    void draw_pixels_to_texture(PixelArr pixels, GLuint* texture);

    void update();
};
