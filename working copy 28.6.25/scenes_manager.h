#pragma once

#include "scene.h"

class MainScene : public Scene {
private:
    d3Object* cube = nullptr;

public:
    void setup() override {
        cube = loadObj("./data.obj");
        cube->transform_scale(50);
        cube->transform_rotation_degrees({0, 0, 0});
    }

    void update(float delta_time) override {
        // ! Bottelneck
        cube->transform_rotation_degrees({90*delta_time, 90*delta_time, 90*delta_time});
    }
};

Scene* get_main_scene() {
    return new MainScene();
}
