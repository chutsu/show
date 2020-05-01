#ifndef SHOW_HPP
#define SHOW_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>

#include <stb/stb_image.h>

namespace show {

#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_ERROR(M, ...)                                                      \
  fprintf(stderr,                                                              \
          "\033[31m[ERROR] [%s:%d] " M "\033[0m\n",                            \
          __FILENAME__,                                                        \
          __LINE__,                                                            \
          ##__VA_ARGS__)

#define LOG_INFO(M, ...) fprintf(stdout, "[INFO] " M "\n", ##__VA_ARGS__)
#define LOG_WARN(M, ...)                                                       \
  fprintf(stdout, "\033[33m[WARN] " M "\033[0m\n", ##__VA_ARGS__)

#define FATAL(M, ...)                                                          \
  fprintf(stdout,                                                              \
          "\033[31m[FATAL] [%s:%d] " M "\033[0m\n",                            \
          __FILENAME__,                                                        \
          __LINE__,                                                            \
          ##__VA_ARGS__);                                                      \
  exit(-1)

#define UNUSED(expr)                                                           \
  do {                                                                         \
    (void) (expr);                                                             \
  } while (0)

/****************************************************************************
 *                                UTILS
 ***************************************************************************/

void print_vec3(const std::string &title, const glm::vec3 &v);
void print_vec4(const std::string &title, const glm::vec4 &v);
void print_mat3(const std::string &title, const glm::mat3 &m);
void print_mat4(const std::string &title, const glm::mat4 &m);

/****************************************************************************
 *                                SHADER
 ***************************************************************************/

int shader_compile(const char *shader_src, const int type);
int shader_compile(const std::string &shader_path, const int type);
int shaders_link(const int vertex_shader,
                 const int fragment_shader,
                 const int geometry_shader = -1);

struct glprog_t {
  unsigned int program_id;

  glprog_t(const std::string &vs_path, const std::string &fs_path);
  glprog_t(const std::string &vs_path,
           const std::string &fs_path,
           const std::string &gs_path);
  glprog_t(const char *vs_src, const char *fs_var);

  void use() const;
  int set(const std::string &key, const bool value) const;
  int set(const std::string &key, const int value) const;
  int set(const std::string &key, const float value) const;
  int set(const std::string &key, const glm::vec2 &value) const;
  int set(const std::string &key, const float x, const float y) const;
  int set(const std::string &key, const glm::vec3 &value) const;
  int set(const std::string &key,
          const float x,
          const float y,
          const float z) const;
  int set(const std::string &key, const glm::vec4 &value) const;
  int set(const std::string &key,
          const float x,
          const float y,
          const float z,
          const float w) const;
  int set(const std::string &key, const glm::mat2 &mat) const;
  int set(const std::string &key, const glm::mat3 &mat) const;
  int set(const std::string &key, const glm::mat4 &mat) const;
};

/****************************************************************************
 *                                 TEXTURE
 ***************************************************************************/

struct gltexture_t {
  unsigned int id;
  std::string type;
  std::string path;
};

unsigned int load_texture(int img_width,
                          int img_height,
                          int img_channels,
                          const unsigned char *data);

unsigned int load_texture(const std::string &texture_file,
                          int &img_width,
                          int &img_height,
                          int &img_channels);

unsigned int load_texture(const std::string &texture_file);

struct glvertex_t {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoords;
  glm::vec3 tangent;
  glm::vec3 bitangent;
};

/****************************************************************************
 *                                 MESH
 ***************************************************************************/

struct glmesh_t {
  std::vector<glvertex_t> vertices;
  std::vector<unsigned int> indices;
  std::vector<gltexture_t> textures;
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;

  glmesh_t(const std::vector<glvertex_t> &vertices_,
           const std::vector<unsigned int> &indices_,
           const std::vector<gltexture_t> &textures_);
};

void glmesh_init(glmesh_t &mesh);
void glmesh_draw(const glmesh_t &mesh, const glprog_t &program);

/****************************************************************************
 *                                 CAMERA
 ***************************************************************************/

enum glcamera_movement_t {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  YAW_LEFT,
  YAW_RIGHT,
  PAN
};

class glcamera_t {
public:
  glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

  // State
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 right = glm::vec3(-1.0f, 0.0f, 0.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
  float yaw = -180.0f;
  float pitch = 0.0f;

  // Settings
  float movement_speed = 50.0f;
  float mouse_sensitivity = 0.05f;
  float fov = 45.0f;
  float near = 0.1f;
  float far = 100.0f;
  int &screen_width;
  int &screen_height;

  glcamera_t(int &screen_width_,
             int &screen_height_,
             const glm::vec3 position_ = glm::vec3(0.0f, 5.0f, 30.0f));
};

glm::mat4 glcamera_projection(const glcamera_t &camera);
glm::mat4 glcamera_view_matrix(const glcamera_t &camera);
void glcamera_update(glcamera_t &camera);
void glcamera_keyboard_handler(glcamera_t &camera,
                               const glcamera_movement_t &direction,
                               const float dt);
void glcamera_mouse_handler(glcamera_t &camera, const float dx, const float dy);
void glcamera_scroll_handler(glcamera_t &camera, const float dy);


/****************************************************************************
 *                                 MODEL
 ***************************************************************************/

namespace shaders {

static const char *glmodel_vs = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

static const char *glmodel_fs = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main() {
  FragColor = texture(texture_diffuse1, TexCoords);
}
)glsl";

} // namespace shaders

struct glmodel_t {
  glprog_t program;

  std::vector<gltexture_t> textures_loaded;
  std::vector<glmesh_t> meshes;
  std::string directory;
  bool gamma_correction = false;

  glm::mat4 T_SM = glm::mat4(1.0f);

  glmodel_t(const std::string &path,
            const std::string &vs,
            const std::string &fs,
            bool gamma = false);

  glmodel_t(const std::string &path,
            const char *vs = shaders::glmodel_vs,
            const char *fs = shaders::glmodel_fs,
            bool gamma = false);
};

void glmodel_draw(glmodel_t &model, const glcamera_t &camera);
void glmodel_load(glmodel_t &model, const std::string &path);
void glmodel_process_node(glmodel_t &model, aiNode *node,
                          const aiScene *scene);
glmesh_t glmodel_process_mesh(glmodel_t &model,
                              aiMesh *mesh,
                              const aiScene *scene);
std::vector<gltexture_t> glmodel_load_textures(glmodel_t &model,
                                               aiMaterial *mat,
                                               aiTextureType type,
                                               std::string typeName);
unsigned int texture_from_file(const std::string &dir,
                               const char *fp,
                               bool gamma = false);

/****************************************************************************
 *                                  DRAW
 ***************************************************************************/

namespace shaders {

static const char *glcf_vs = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

static const char *glcf_fs = R"glsl(
#version 150 core

out vec4 FragColor;

void main() {
  FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
)glsl";

static const char *glcube_vs = R"glsl(
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(in_pos, 1.0);
  color = in_color;
}
)glsl";

static const char *glvoxel_vs = R"glsl(
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(in_pos, 1.0);
  color = in_color;
}
)glsl";

static const char *glvoxel_fs = R"glsl(
#version 150 core
in vec3 color;
out vec4 frag_color;

void main() {
  frag_color = vec4(color, 1.0f);
}
)glsl";


static const char *glcube_fs = R"glsl(
#version 150 core
in vec3 color;
out vec4 frag_color;

void main() {
  frag_color = vec4(color, 1.0f);
}
)glsl";

static const char *glframe_vs = R"glsl(
#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(in_pos, 1.0);
  color = in_color;
}
)glsl";

static const char *glframe_fs = R"glsl(
#version 150 core
in vec3 color;
out vec4 frag_color;

void main() {
  frag_color = vec4(color, 1.0f);
}
)glsl";

static const char *glgrid_vs = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

static const char *glgrid_fs = R"glsl(
#version 150 core

out vec4 FragColor;

void main() {
  FragColor = vec4(0.8f, 0.8f, 0.8f, 1.0f);
}
)glsl";

static const char *glplane_vs = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main() {
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)glsl";

static const char *glplane_fs = R"glsl(
#version 330 core

in vec3 ourColor;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
	FragColor = texture(texture1, TexCoord);
}
)glsl";

} // namespace shaders

class globj_t {
public:
  glprog_t program_;
  unsigned int VAO_;
  unsigned int VBO_;
  unsigned int EBO_;
  glm::mat4 T_SM_ = glm::mat4(1.0f);

  globj_t(const char *vs, const char *fs);

  void pos(const glm::vec3 &pos);
  glm::vec3 pos();
  glm::mat3 rot();
};

class glcf_t : public globj_t {
public:
  float fov_ = glm::radians(60.0f);
  float scale_ = 1.0;
  float line_width_ = 2.0f;

  glcf_t();
  ~glcf_t();
  void draw(const glcamera_t &camera);
};

class glcube_t : public globj_t {
public:
  const float cube_size_ = 0.5;
  glm::vec3 color_{0.9, 0.4, 0.2};

  glcube_t();
  glcube_t(const float cube_size);
  ~glcube_t();
  void draw(const glcamera_t &camera);
};

class glframe_t : public globj_t {
public:
  const float line_width_ = 5.0f;

  glframe_t();
  ~glframe_t();
  void draw(const glcamera_t &camera);
};

class glgrid_t : public globj_t {
public:
  const int grid_size_ = 10;

  glgrid_t();
  ~glgrid_t();
  void draw(const glcamera_t &camera);
};

class glplane_t : public globj_t {
public:
  std::string image_path_;
  float width_ = 0.5;
  float height_ = 0.5;
  glm::vec3 color_{0.9, 0.4, 0.2};

  int img_width_ = 0;
  int img_height_ = 0;
  int img_channels_ = 0;

  unsigned int texture_;
  GLuint FBO_;

  glplane_t(const std::string &image_path);
  void draw(const glcamera_t &camera);
};

class glvoxels_t : public globj_t {
public:
  std::vector<GLfloat> vertices;

  glvoxels_t(const float voxel_size, const size_t max_voxels);
  ~glvoxels_t();
  void draw(const glcamera_t &camera);
};

/****************************************************************************
 *                                 GUI
 ***************************************************************************/

class gui_t {
public:
  bool keep_running_ = true;
  GLFWwindow *gui_;
  const std::string title_;
  int width_;
  int height_;
  float time_last_;
  float dt_;
  ImVec4 clear_color_{0.45f, 0.55f, 0.60f, 1.00f};
  glcamera_t camera{width_, height_};

	// Mouse
	bool right_click = false;
	bool left_click = false;
	bool last_cursor_set = false;
	double last_cursor_x = 0.0;
	double last_cursor_y = 0.0;

  gui_t(const std::string &title, const int width, const int height);
  gui_t(const std::string &title);
  ~gui_t();

  static void error_callback(int error, const char *description);
  static void key_callback(GLFWwindow* window,
                           int key,
                           int scancode,
                           int action,
                           int mods);
  static void window_callback(GLFWwindow *window,
                              const int width,
                              const int height);
  static void mouse_cursor_callback(GLFWwindow *window,
																		const double xpos,
																		const double ypos);
  static void mouse_button_callback(GLFWwindow *window,
																		int button,
																		int action,
																		int mods);
	static void mouse_scroll_callback(GLFWwindow * window,
																		double xoffset,
																		double yoffset);

  bool ok();
  void poll();
  void clear();
  void render(const bool clear_gui = false);
};

class gui_imshow_t {
public:
  bool ok_ = false;

  std::string title_;
  GLuint FBO_;
  GLuint RBO_;

  std::string img_path_;
  int img_width_ = 0;
  int img_height_ = 0;
  int img_channels_ = 0;
  GLuint img_id_;

  gui_imshow_t(const std::string &title);
  gui_imshow_t(const std::string &title, const std::string &img_path);
  gui_imshow_t(const std::string &title,
               const int img_width,
               const int img_height,
               const int img_channels,
               const unsigned char *data);

  void init(const std::string &title,
            const int img_width,
            const int img_height,
            const int img_channels,
            const unsigned char *data);

  bool ok();
  void update(void *pixels);
  void show();
  void show(const int img_width,
            const int img_height,
            const int img_channels,
            const unsigned char *data);
};

} // namespace show
#endif // SHOW_HPP
