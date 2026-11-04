
#include <iostream>
#include <GLFW/glfw3.h>

#include "scene.h"

#define START_SCENE_INDEX 1

class MainScene : public Scene {
private:
    // private vars
public:
    void setup() override {
        // is_static = true;
        load_object("./data.obj")->transform_scale(10)->transform_position({-30,0,-10});
        load_object("./data.obj")->transform_scale(10)->transform_position({-5,-20,-20});
        load_object("./data.obj")->transform_scale(10)->transform_position({-15,15,0});
        
        load_object("./data.obj")->transform_scale({50,2,50})->transform_position({0,-30,0});
    }

    void update(float delta_time) override {
        find_by_index(0)->transform_position({0,-20*delta_time, 0});
        find_by_index(1)->transform_position({0,-20*delta_time, 0});
        find_by_index(2)->transform_position({0,-10*delta_time, 0});
        find_by_index(0)->transform_rotation_degrees({90*delta_time, 90*delta_time, 90*delta_time});
        find_by_index(1)->transform_rotation_degrees({-90*delta_time, -90*delta_time, -90*delta_time});
    }
};

class Monkey : public Scene {
private:
    float scale = .1;
    int half_size = 50;
    float time = 0;
    Vec2 initial_mouse_pos;

public:
    void setup() override {
        // is_static = true;
        set_mouse_visability(false);
        initial_mouse_pos = get_mouse_position();

        camera.skybox_from_png("/Users/jakobsauer/Downloads/free-skybox-basic-sky/textures/sky_water_landscape.jpg");
        camera.position = {0, 50, 200};
        // camera.rotate_degrees_y(180);
        add_light({degrees2radians(45.0), degrees2radians(45.0), 0}, 255);
        load_object("./data_monkey.obj")->set_reflective_index(2)->transform_scale(30)->transform_position({0,50,-10});

        d3Object* plane = empty_object()->set_name("plane")->transform_scale(30)->transform_position({0,-50,-10});

        // plane->add_vertex({0,0,0});
        // plane->add_vertex({1,0,0});
        // plane->add_vertex({0,0,1});
        // plane->add_face({0,2,1}, {0,1,0});

        for (int y = -half_size; y <  half_size; y++)
        {
            for (int x = -half_size; x <  half_size; x++)
            {
                plane->add_vertex({
                    x * scale,
                    0,
                    y * scale,
                });
            }
        }
        for (int y = 0; y < half_size*2-1; y++)
        {
            for (int x = 0; x < half_size*2-1; x++)
            {
                plane->add_face({
                    x + half_size * y * 2,
                    x + half_size * (y+1) * 2,
                    (x+1) + half_size * y * 2,
                });
                plane->add_face({
                    (x+1) + half_size * (y+1) * 2,
                    (x+1) + half_size * y * 2,
                    x + half_size * (y+1) * 2,
                });
            }
        }
    }

    void update(float delta_time) override {
        d3Object* plane = find_by_name("plane")[0];
        for (int z = 0; z < half_size * 2; z++) {
            for (int x = 0; x < half_size * 2; x++) {
                float fx = (float)x / (half_size * 2);
                float fz = (float)z / (half_size * 2);
                int i = z * (half_size * 2) + x;
                float vertex_y = 0;
                vertex_y = vertex_y + std::sinf(fx * 10 * M_PI) * std::sinf(3.0f*time);
                vertex_y = vertex_y + std::sinf(fz * 10 * M_PI) * std::sinf(3.0f*time);
                plane->vertices[i].y = vertex_y;
            }
        }
        plane->refresh();
        time = time + delta_time;
        plane->transform_rotation_degrees({0*delta_time,45*delta_time,0*delta_time});

        const int move_speed = 50;
        Vec3 dir = {0,0,0};
        if (is_key_pressed(GLFW_KEY_W)) dir.z = dir.z - 1;
        if (is_key_pressed(GLFW_KEY_S)) dir.z = dir.z + 1;
        if (is_key_pressed(GLFW_KEY_D)) dir.x = dir.x + 1;
        if (is_key_pressed(GLFW_KEY_A)) dir.x = dir.x - 1;
        if (is_key_pressed(GLFW_KEY_SPACE)) dir.y = dir.y + 1;
        if (is_key_pressed(GLFW_KEY_LEFT_SHIFT)) dir.y = dir.y - 1;
        dir.rotate_order_reversed({0, camera.rotation.y, 0});
        dir = dir * (move_speed*delta_time);
        camera.move(dir);

        if (is_key_pressed(GLFW_KEY_ESCAPE)) quit();
        if (is_key_pressed(GLFW_KEY_R)) {
            camera.rotation = {0,0,0};
            camera.position = {0,0,100};
        }

        const float mouse_sensitivity = 0.005;
        Vec2 mouse_pos = get_mouse_position();
        camera.rotation.y = -(mouse_pos.x - initial_mouse_pos.x) * mouse_sensitivity;
        camera.rotation.x = -(mouse_pos.y - initial_mouse_pos.y) * mouse_sensitivity;
        camera.rotation.x = std::max(camera.rotation.x, (float)-M_PI/2.0f);
        camera.rotation.x = std::min(camera.rotation.x, (float)M_PI/2.0f);
        if (is_key_pressed(GLFW_KEY_G)) camera.rotate_degrees_z(move_speed*delta_time);
        if (is_key_pressed(GLFW_KEY_H)) camera.rotate_degrees_z(-move_speed*delta_time);

        // lights[0].direction.rotate({0,degrees2radians(270)*delta_time,0});
    }
};