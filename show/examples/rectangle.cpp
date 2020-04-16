#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <assert.h>

// GLOBAL VARIABLES
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;
const char *TITLE = "Rectangle Example";
float dt = 0.0f;
float frame_last = 0.0f;
bool first_mouse = true;

const char *vertex_shader_src = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)glsl";

const char *fragment_shader_src = R"glsl(
#version 330 core
out vec4 FragColor;
void main() {
   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)glsl";

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  // Make sure the viewport matches the new window dimensions
  glViewport(0, 0, width, height);
}

GLFWwindow *create_window() {
  // Initialize GLFW
  if (!glfwInit()) {
    return NULL;
  }

  // Create a windowed mode window and its OpenGL context
  GLFWwindow *window;
  window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, NULL, NULL);
  if (!window) {
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return NULL;
  }

  return window;
}

int compile_shader(const char *shader_src, const int &type) {
  int shader = glCreateShader(type);
  glShaderSource(shader, 1, &shader_src, NULL);
  glCompileShader(shader);

  int success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(shader, 512, NULL, log);
    printf("Failed to compile shader:\n%s\n", log);
    exit(-1);
  }

  return shader;
}

int link_shaders(const int vertex_shader, const int fragment_shader) {
  // Attach shaders to link
  int program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  // Link program
  int success = 0;
  char log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, log);
    printf("Failed to link shaders: %s\n", log);
    exit(-1);
  }

  // Delete shaders
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return program;
}

int main() {
  // Create window
  GLFWwindow *window = create_window();
  if (!window) {
    glfwTerminate();
    return -1;
  }

  // Build and compile our shader program
  int vertex_shader = compile_shader(vertex_shader_src, GL_VERTEX_SHADER);
  int fragment_shader = compile_shader(fragment_shader_src, GL_FRAGMENT_SHADER);
  int shader_program = link_shaders(vertex_shader, fragment_shader);

  // Set up vertex data (and buffer(s)) and configure vertex attributes
  // clang-format off
  float vertices[] = {
    0.5f, 0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
  };
  unsigned int indices[] = {
    // Note that we start from 0!
    0, 1, 3, // first Triangle
    1, 2, 3 // second Triangle
  };
  // clang-format on

  // -- VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  // -- VBO
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        3 * sizeof(float),
                        (void *) 0);
  glEnableVertexAttribArray(0);
  // -- EBO
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(indices),
               indices,
               GL_STATIC_DRAW);
  // -- Clean up
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Uncomment this call to draw in wireframe polygons.
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    process_input(window);

    // Clear screen
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw our first triangle
    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // Unbind VAO

    // Swap buffers and poll IO events (keyboard, mouse, etc)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}
