CXX = clang++

SOURCE = main.cpp objects.cpp scene.cpp BVH.cpp world.cpp
TARGET = d3_renderer

CXXFLAGS = -O3 -std=c++14 -w
INCLUDES = -I./glfw/include

LIBS = ./glfw/lib-arm64/libglfw3.a

LDFLAGS = -Wl,-rpath,@executable_path/glfw/lib-arm64 \
          -framework Cocoa \
          -framework OpenGL \
          -framework IOKit \
          -framework CoreVideo

release:
	$(CXX) $(SOURCE) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS) -o $(TARGET)
	./$(TARGET)

debug:
	$(CXX) $(SOURCE) -O0 -g -Wall -Wextra -std=c++14 $(INCLUDES) $(LIBS) $(LDFLAGS) -o $(TARGET)
	./$(TARGET)

.PHONY: release debug
