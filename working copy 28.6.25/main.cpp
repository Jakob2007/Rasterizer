
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <iostream>

#include "scenes_manager.h"

/* --- compile command ---
cd documents/development/c++/renderer

clang++ main.cpp rasterizer.cpp scene.cpp \
  -I./glfw/include \
  ./glfw/lib-arm64/libglfw3.a \
  -Wl,-rpath,@executable_path/glfw/lib-arm64 \
  -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo \
  -std=c++11 \
  -o d3_renderer; ./d3_renderer

  ./d3_renderer
*/

const int WIDTH = 800;
const int HEIGHT = 600;
const char* title = "Rasterizer";

void init_texture(GLuint* texture) {
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, nullptr); // allocate once
}

void draw_pixels_to_texture(unsigned char* pixels, GLuint* texture) {
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT,
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

int main() {
	if (!glfwInit()) return -1;

	
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, title, NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// load scene
	Scene* current_scene = get_main_scene();
	current_scene->setup();

	// RGBA format: 4 bytes per pixel
	unsigned char* pixels = new unsigned char[WIDTH * HEIGHT * 4];

	GLuint texture;
	init_texture(&texture);

	auto last_update = std::chrono::high_resolution_clock::now();
	float delta_time = 0;

	while (!glfwWindowShouldClose(window)) {
		
		current_scene->update(delta_time);

		current_scene->render(pixels, WIDTH, HEIGHT);
		
		draw_pixels_to_texture(pixels, &texture);
		glfwSwapBuffers(window);
		glfwPollEvents();

		// fps handling
		const auto& now = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();
		last_update = now;
		
		float fps = 1000.0 / duration;
		delta_time = duration / 1000.0;

		std::string sub_title = std::string(title) + " - FPS: " + std::to_string(fps) + " - duration: " + std::to_string(duration);
		glfwSetWindowTitle(window, sub_title.c_str());
	}

	delete current_scene;
	delete[] pixels;
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}



