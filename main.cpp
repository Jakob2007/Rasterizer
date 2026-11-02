
#include <iostream>

#include "world.h"

/* --- compile command ---
cd /Users/jakobsauer/Documents/Development/C++/renderer/; clear

clang++ main.cpp objects.cpp scene.cpp BVH.cpp world.cpp -O3 \
  -I./glfw/include \
  ./glfw/lib-arm64/libglfw3.a \
  -Wl,-rpath,@executable_path/glfw/lib-arm64 \
  -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo \
  -std=c++14 \
  -o d3_renderer; ./d3_renderer

  ./d3_renderer
*/

/*
TODO:

real depth map
fix BVH rendering (likely deeper issue)
create material class
implement point light (prolly not feasable)
create depth map
create ui interface
reimplement reflective index
*/

int main() {
	World world;
	world.setup();

	while (world.running) {
		world.update();
	}

	world.terminate();
}



