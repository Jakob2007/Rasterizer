# CPU-Rasterizer
### 3d Game Engine

## Usage Overview:
# General
- The World is structured in World -> Scenes -> Object -> Geometry
- Scenes are located in scene_design.h and may be altered
- compile using make

# Flags
The starting scene can be set in scene_design.h
- START_SCENE_INDEX
Debugging flags can be set in config.h
- DRAW_BARYCENTRIC_COORDS
- DRAW_BOUNDING_BOX
- DRAW_NORMAL_COLOR
- DISABLE_BACKFACE_CULLING
- DRAW_BVH
- DRAW_SKYBOX_GRID

## Scene Design
Scenes are classes in scene_design.h inheriting from Scene
Scenes may overwrite update(float delta_time) and setup()
# Shortend selection of usable functionality

Scene:
- d3Object* load_object(const std::string& filename);
- std::vector<d3Object*> find_by_name(const std::string& filename);

- void set_ambient_light(RGBAColor col)
- void add_light(Vec3 dir, RGBAColor col)

- int is_key_pressed(int key)
- Vec2 get_mouse_position()
- void set_mouse_visability(bool is_visable)

Objects (suports method chaining):
- d3Object* transform_position(Vec3 delta_pos)
- d3Object* transform_rotation(Vec3 delta_rot)
- d3Object* transform_scale(Vec3 delta_sca)
- d3Object* add_face(std::array<int, 3> face)
- d3Object* set_color(RGBAColor new_color)

Camera:
- void move(Vec3 delta) -> relative movement
- void rotate(Vec3 delta) -> relative rotation
- void set_fov_from_degrees(float value, float aspect_ratio)
- void skybox_from_png(const char* filename)

Vekor-like Objects support basic arithmetic 


