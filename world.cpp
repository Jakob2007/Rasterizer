
#include "world.h"

void World::setup() {
    // Graphics
    window_init();
    glfwSetWindowUserPointer(window, this);
    pixels = new unsigned char[width * height * 4]; // ? can this be on the stack?
    texture_init(&texture);

    // Input
    // glfwSetKeyCallback(window, World::manage_input); // * alternative input management

    // Timing
    last_update = std::chrono::high_resolution_clock::now();

    // Scenes
    for (int i = 0; i < scene_count; i++)
    {
        scenes[i]->world = this;
        scenes[i]->camera.set_fov_from_degrees(90, height / (float)width);
    }

    active_scene = scenes[start_scene_index].get();
    active_scene->setup();
}

void World::terminate() {
    // for (size_t i = 0; i < scene_count; i++) delete scenes[i];
    delete[] pixels;
    glfwDestroyWindow(window);
    glfwTerminate();
}

void World::update_scene() {
    if (!active_scene->is_static) active_scene->update(current_delta_time);
    active_scene->render(pixels, width, height);
}

float World::set_delta_time_and_get_fps() {
    const auto& now = std::chrono::high_resolution_clock::now();
    float frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();
    last_update = now;
    
    current_delta_time = frame_duration / 1000.0;
    durations[idx] = frame_duration;
    idx = (idx + 1) % fps_smoothing;

    int sum = 0;
    for (int i = 0; i < fps_smoothing; ++i) sum += durations[i];
    return sum == 0 ? 0 : round(1000.0 / (sum / fps_smoothing)*100.0)/100.0;
}

void World::window_init() {
    if (!glfwInit()) throw "glfw init failed";
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw "window creation failed";
    }
    glfwMakeContextCurrent(window);
}

void World::texture_init(GLuint* texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void World::draw_pixels_to_texture(PixelArr pixels, GLuint* texture) {
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-1, -1);
    glTexCoord2f(1, 0); glVertex2f( 1, -1);
    glTexCoord2f(1, 1); glVertex2f( 1,  1);
    glTexCoord2f(0, 1); glVertex2f(-1,  1);
    glEnd();
}

void World::update() {
    const auto& rasterization_start = std::chrono::high_resolution_clock::now();

    update_scene();
    if (!running) return;

    draw_pixels_to_texture(pixels, &texture);
    glfwSwapBuffers(window);
    glfwPollEvents();

    const auto& rasterization_end = std::chrono::high_resolution_clock::now();
    float render_duration = std::chrono::duration_cast<std::chrono::milliseconds>(rasterization_end - rasterization_start).count();

    float fps = set_delta_time_and_get_fps();
    std::string sub_title = std::string(title) + " - FPS: " + std::to_string(fps) + " - render duration: " + std::to_string(render_duration);
    glfwSetWindowTitle(window, sub_title.c_str());
    if(glfwWindowShouldClose(window)) {running = false;}
}
