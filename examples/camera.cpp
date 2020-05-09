#include <iostream>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// GLOBAL VARIABLES
bool keep_running = true;
int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;
const char *TITLE = "Camera Example";
float dt = 0.0f;
float frame_last = 0.0f;
bool first_mouse = true;

void print_mat4(const std::string &title, const glm::mat4 &m) {
  const glm::vec4 c1 = m[0];
  const glm::vec4 c2 = m[1];
  const glm::vec4 c3 = m[2];
  const glm::vec4 c4 = m[3];

  printf("%s:\n", title.c_str());
  printf("%f, %f, %f, %f\n", c1.x, c2.x, c3.x, c4.x);
  printf("%f, %f, %f, %f\n", c1.y, c2.y, c3.y, c4.y);
  printf("%f, %f, %f, %f\n", c1.z, c2.z, c3.z, c4.z);
  printf("%f, %f, %f, %f\n", c1.w, c2.w, c3.w, c4.w);
}

/****************************************************************************
 *                               SHADER
 ***************************************************************************/

int shader_compile(const char *shader_src, const int type) {
  assert(shader_src != nullptr);

  int shader = glCreateShader(type);
  glShaderSource(shader, 1, &shader_src, NULL);
  glCompileShader(shader);

  int success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(shader, 512, NULL, log);
    printf("Failed to compile fragment shader:\n%s\n", log);
    return -1;
  }

  return shader;
}

int shaders_link(const int vertex_shader,
                 const int fragment_shader,
                 const int geometry_shader=-1) {
  assert(vertex_shader != -1);
  assert(fragment_shader != -1);

  // Attach shaders to link
  int program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  if (geometry_shader != -1) {
    glAttachShader(program, geometry_shader);
  }
  glLinkProgram(program);

  // Link program
  int success = 0;
  char log[1024];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 1024, NULL, log);
    printf("Failed to link shaders:\nReason: %s\n", log);
    exit(-1);
  }

  // Delete shaders
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  if (geometry_shader == -1) {
    glDeleteShader(geometry_shader);
  }

  return program;
}

/****************************************************************************
 *                                CAMERA
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

struct glcamera_t {
  // State
  glm::vec3 focal_point = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 position = glm::vec3(0.0f, 5.0f, 10.0f);
  glm::vec3 right = glm::vec3(-1.0f, 0.0f, 0.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
  float yaw = M_PI / 4.0;
  float pitch = glm::radians(-35.0f);

  // Settings
  float movement_speed = 50.0f;
  float mouse_sensitivity = 0.02f;
  float fov = 1.0472f;
  float near = 0.1f;
  float far = 100.0f;
  int &screen_width;
  int &screen_height;

  glcamera_t(int &screen_width_, int &screen_height_)
      : screen_width{screen_width_}, screen_height{screen_height_} {
    update();
  }

  void update() {
    // Calculate the new front vector
    front.x = sin(yaw) * cos(pitch);
    front.y = sin(pitch);
    front.z = cos(yaw) * cos(pitch);
    front = glm::normalize(front);

    // Also re-calculate the right and Up vector
    right = glm::normalize(glm::cross(front, world_up));

    // Normalize the vectors, because their length gets closer to 0 the more
    // you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
  }

  glm::mat4 projection() const {
    const float ratio = (float) screen_width / (float) screen_height;
    return glm::perspective(fov, ratio, near, far);
  }

  glm::mat4 view() const {
    float radius = 10.0;

    float eye_x = focal_point.x + radius * sin(yaw);
    float eye_y = focal_point.y + radius * cos(pitch);
    float eye_z = focal_point.z + radius * cos(yaw);
    glm::vec3 eye{eye_x, eye_y, eye_z};

    // printf("eye: %f %f %f\n", eye_x, eye_y, eye_z);
    // printf("pitch: %f  ", pitch);
    // printf("yaw: %f\n", yaw);
    return glm::lookAt(eye, focal_point, world_up);
  }

  void keyboard_handler(const glcamera_movement_t &direction,
                        const float dt) {
    float velocity = movement_speed * dt;
    if (direction == FORWARD) {
      position += front * velocity;
    }
    if (direction == BACKWARD) {
      position -= front * velocity;
    }
    if (direction == LEFT) {
      position -= right * velocity;
    }
    if (direction == RIGHT) {
      position += right * velocity;
    }
    if (direction == UP) {
      position += up * velocity;
    }
    if (direction == DOWN) {
      position -= up * velocity;
    }
  }

  void rotate(const float dx, const float dy) {
    // Update yaw and pitch
    yaw -= dx * mouse_sensitivity;
    pitch += dy * mouse_sensitivity;

    // Constrain pitch and yaw
    pitch = (pitch < -M_PI / 2.0) ? -M_PI / 2.0 : pitch;
    pitch = (pitch > 0.0) ? 0.0 : pitch;
    yaw = (yaw > M_PI) ? yaw - 2 * M_PI : yaw;
    yaw = (yaw < -M_PI) ? yaw + 2 * M_PI : yaw;

    // Update camera attitude
    update();
  }

	void pan(const float dx, const float dy) {
		position -= front * (dy * mouse_sensitivity);
		position += right * (dx * mouse_sensitivity);
		focal_point -= front * (dy * mouse_sensitivity);
		focal_point += right * (dx * mouse_sensitivity);
		update();
	}

  void zoom(const float dy) {
    if (fov >= glm::radians(0.5f) && fov <= glm::radians(90.0f)) {
      fov -= dy * 0.1;
    }

		if (fov <= glm::radians(0.5f)) {
      fov = glm::radians(5.0f);
    } else if (fov >= glm::radians(90.0f)) {
      fov = glm::radians(90.0f);
    }
  }
};

/*****************************************************************************
 *                                CUBE
 ****************************************************************************/

struct cube_t {
  const char *glcube_vs = R"glsl(
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

  const char *glcube_fs = R"glsl(
#version 150 core
in vec3 color;
out vec4 frag_color;

void main() {
  frag_color = vec4(color, 1.0f);
}
)glsl";

  const float cube_size = 0.5;
  glm::vec3 color{0.9, 0.4, 0.2};
  glm::mat4 T_MS = glm::mat4(1.0f);

  int program_id = 0;
  unsigned int VAO = 0;
  unsigned int VBO = 0;

  cube_t() {
    const int vs = shader_compile(glcube_vs, GL_VERTEX_SHADER);
    const int fs = shader_compile(glcube_fs, GL_FRAGMENT_SHADER);
    program_id = shaders_link(vs, fs);

    // Vertices
    // clang-format off
    const float r = color.x;
    const float g = color.y;
    const float b = color.z;
    static const GLfloat vertices[] = {
      // Triangle 1
      -cube_size, -cube_size, -cube_size, r, g, b,
      -cube_size, -cube_size, cube_size, r, g, b,
      -cube_size, cube_size, cube_size, r, g, b,
      // Triangle 2
      cube_size, cube_size, -cube_size, r, g, b,
      -cube_size, -cube_size, -cube_size, r, g, b,
      -cube_size, cube_size, -cube_size, r, g, b,
      // Triangle 3
      cube_size, -cube_size, cube_size, r, g, b,
      -cube_size, -cube_size, -cube_size, r, g, b,
      cube_size, -cube_size, -cube_size, r, g, b,
      // Triangle 4
      cube_size, cube_size, -cube_size, r, g, b,
      cube_size, -cube_size, -cube_size, r, g, b,
      -cube_size, -cube_size, -cube_size, r, g, b,
      // Triangle 5
      -cube_size, -cube_size, -cube_size, r, g, b,
      -cube_size, cube_size, cube_size, r, g, b,
      -cube_size, cube_size, -cube_size, r, g, b,
      // Triangle 6
      cube_size, -cube_size, cube_size, r, g, b,
      -cube_size, -cube_size, cube_size, r, g, b,
      -cube_size, -cube_size, -cube_size, r, g, b,
      // Triangle 7
      -cube_size, cube_size, cube_size, r, g, b,
      -cube_size, -cube_size, cube_size, r, g, b,
      cube_size, -cube_size, cube_size, r, g, b,
      // Triangle 8
      cube_size, cube_size, cube_size, r, g, b,
      cube_size, -cube_size, -cube_size, r, g, b,
      cube_size, cube_size, -cube_size, r, g, b,
      // Triangle 9
      cube_size, -cube_size, -cube_size, r, g, b,
      cube_size, cube_size, cube_size, r, g, b,
      cube_size, -cube_size, cube_size, r, g, b,
      // Triangle 10
      cube_size, cube_size, cube_size, r, g, b,
      cube_size, cube_size, -cube_size, r, g, b,
      -cube_size, cube_size, -cube_size, r, g, b,
      // Triangle 11
      cube_size, cube_size, cube_size, r, g, b,
      -cube_size, cube_size, -cube_size, r, g, b,
      -cube_size, cube_size, cube_size, r, g, b,
      // Triangle 12
      cube_size, cube_size, cube_size, r, g, b,
      -cube_size, cube_size, cube_size, r, g, b,
      cube_size, -cube_size, cube_size, r, g, b
      // Triangle 12 : end
    };
    const size_t nb_triangles = 12;
    const size_t vertices_per_triangle = 3;
    const size_t nb_vertices = vertices_per_triangle * nb_triangles;
    const size_t vertex_buffer_size = sizeof(float) * 6 * nb_vertices;
    // clang-format on

    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertices, GL_STATIC_DRAW);
    // -- Position attribute
    size_t vertex_size = 6 * sizeof(float);
    void *pos_offset = (void *) 0;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, pos_offset);
    glEnableVertexAttribArray(0);
    // -- Color attribute
    void *color_offset = (void *) (3 * sizeof(float));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, color_offset);
    glEnableVertexAttribArray(1);

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindVertexArray(0);             // Unbind VAO
  }

  void draw(const glcamera_t &camera) {
    glUseProgram(program_id);

    // Set projection matrix
    {
      const auto location = glGetUniformLocation(program_id, "projection");
      if (location == -1) {
        printf("Failed to get [projection] location in program!\n");
      }
      glUniformMatrix4fv(location, 1, GL_FALSE, &camera.projection()[0][0]);
    }

    // Set view matrix
    {
      const auto location = glGetUniformLocation(program_id, "view");
      if (location == -1) {
        printf("Failed to get [view] location in program!\n");
      }
      glUniformMatrix4fv(location, 1, GL_FALSE, &camera.view()[0][0]);
    }

    // Set model matrix
    {
      const auto model = T_MS;
      const auto location = glGetUniformLocation(program_id, "model");
      if (location == -1) {
        printf("Failed to get [model] location in program!\n");
      }
      glUniformMatrix4fv(location, 1, GL_FALSE, &model[0][0]);
    }

    // 12 x 3 indices starting at 0 -> 12 triangles -> 6 squares
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0); // Unbind VAO
  }
};

/****************************************************************************
 *                                  GUI
 ***************************************************************************/

void framebuffer_size_cb(GLFWwindow *window, int width, int height);
void mouse_cursor_cb(GLFWwindow *window, double x, double y);
void mouse_button_cb(GLFWwindow *window, int button, int action, int mods);
void mouse_scroll_cb(GLFWwindow * window, double dx, double dy);

struct gui_t {
  std::string title = "";
  int screen_w = 0;
  int screen_h = 0;
  GLFWwindow *window;
  glcamera_t camera{screen_w, screen_h};

  bool right_click = false;
  bool left_click = false;
  bool last_cursor_set = false;
  double last_cursor_x = 0.0;
  double last_cursor_y = 0.0;

  gui_t(const std::string &title_,
        const int screen_w_,
        const int screen_h_)
      : title{title_},
        screen_w{screen_w_},
        screen_h{screen_h_} {
    // Initialize GLFW
    if (!glfwInit()) {
      std::cout << "Failed to initialize GLFW!" << std::endl;
      exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(screen_w, screen_h, title.c_str(), NULL, NULL);
    if (!window) {
      std::cout << "Failed to create GLFW window!" << std::endl;
      glfwTerminate();
      exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_cb);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouse_cursor_cb);
    glfwSetMouseButtonCallback(window, mouse_button_cb);
    glfwSetScrollCallback(window, mouse_scroll_cb);

    // Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD!" << std::endl;
      exit(-1);
    }
  }

  static void process_input(GLFWwindow *window) {
    gui_t *gui = reinterpret_cast<gui_t *>(glfwGetWindowUserPointer(window));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      gui->camera.keyboard_handler(FORWARD, dt);
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      gui->camera.keyboard_handler(BACKWARD, dt);
    } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      gui->camera.keyboard_handler(LEFT, dt);
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      gui->camera.keyboard_handler(RIGHT, dt);
    }
  }

};

void framebuffer_size_cb(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_cursor_cb(GLFWwindow *window, double x, double y) {
  gui_t *gui = reinterpret_cast<gui_t *>(glfwGetWindowUserPointer(window));
	const float dx = x - gui->last_cursor_x;
	const float dy = y - gui->last_cursor_y;
	gui->last_cursor_x = x;
	gui->last_cursor_y = y;

	// Rotate camera
  if (gui->left_click) {
    if (gui->last_cursor_set == false) {
      gui->last_cursor_set = true;
    } else if (gui->last_cursor_set) {
      gui->camera.rotate(dx, dy);
    }
  }

	// Pan camera
	if (gui->right_click) {
    if (gui->last_cursor_set == false) {
      gui->last_cursor_set = true;
    } else if (gui->last_cursor_set) {
      gui->camera.pan(dx, dy);
    }
  }

	// Reset cursor
  if (gui->left_click == false && gui->right_click == false) {
    gui->last_cursor_set = false;
    gui->last_cursor_x = 0.0;
    gui->last_cursor_y = 0.0;
  }
}

void mouse_button_cb(GLFWwindow *window, int button, int action, int mods) {
  gui_t *gui = reinterpret_cast<gui_t *>(glfwGetWindowUserPointer(window));

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    gui->left_click = (action == GLFW_PRESS) ? true : false;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    gui->right_click = (action == GLFW_PRESS) ? true : false;
  }
}

void mouse_scroll_cb(GLFWwindow *window, double dx, double dy) {
  gui_t *gui = reinterpret_cast<gui_t *>(glfwGetWindowUserPointer(window));
  gui->camera.zoom(dy);
}

/*****************************************************************************
 *                                MAIN
 ****************************************************************************/

int main(void) {
  gui_t gui{TITLE, SCREEN_WIDTH, SCREEN_HEIGHT};
  cube_t cube;

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(gui.window) && keep_running) {
    // Calculate dt
    float frame_now = glfwGetTime();
    dt = frame_now - frame_last;
    frame_last = frame_now;

    // Clear
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Process user input
    glfwPollEvents();
    gui_t::process_input(gui.window);

    // Draw
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe mode
    cube.draw(gui.camera);

    // Swap buffers and poll IO events (keyboard, mouse, etc)
    glEnable(GL_CULL_FACE);
    glfwSwapBuffers(gui.window);
  }

  // Clean up
  glfwTerminate();
  return 0;
}
