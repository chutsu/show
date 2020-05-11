#include "show.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif // STB_IMAGE_IMPLEMENTATION

namespace show {

void print_vec3(const std::string &title, const glm::vec3 &v) {
  printf("%s: ", title.c_str());
  printf("%f, %f, %f\n", v.x, v.y, v.z);
}

void print_vec4(const std::string &title, const glm::vec4 &v) {
  printf("%s: ", title.c_str());
  printf("%f, %f, %f, %f\n", v.x, v.y, v.z, v.w);
}

void print_mat3(const std::string &title, const glm::mat3 &m) {
  const glm::vec3 c1 = m[0];
  const glm::vec3 c2 = m[1];
  const glm::vec3 c3 = m[2];

  printf("%s:\n", title.c_str());
  printf("%f, %f, %f\n", c1.x, c2.x, c3.x);
  printf("%f, %f, %f\n", c1.y, c2.y, c3.y);
  printf("%f, %f, %f\n", c1.z, c2.z, c3.z);
}

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

/*****************************************************************************
 *                                SHADER
 ****************************************************************************/

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

int shader_compile(const std::string &shader_path, const int type) {
  // Open shader file
  std::ifstream shader_file{shader_path};
  if (shader_file.good() == false) {
    return -1;
  }

  // Parse file to string
  std::stringstream ss;
  ss << shader_file.rdbuf();
  shader_file.close();
  const std::string str = ss.str();
  const char *shader_src = str.c_str();

  return shader_compile(shader_src, type);
}

int shaders_link(const int vertex_shader,
                 const int fragment_shader,
                 const int geometry_shader) {
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

glprog_t::glprog_t(const std::string &vs_path, const std::string &fs_path) {
  const int vs = shader_compile(vs_path, GL_VERTEX_SHADER);
  const int fs = shader_compile(fs_path, GL_FRAGMENT_SHADER);
  program_id = shaders_link(vs, fs);
}

glprog_t::glprog_t(const std::string &vs_path,
                   const std::string &fs_path,
                   const std::string &gs_path) {
  const int vs = shader_compile(vs_path, GL_VERTEX_SHADER);
  const int fs = shader_compile(fs_path, GL_FRAGMENT_SHADER);
  const int gs = shader_compile(gs_path, GL_GEOMETRY_SHADER);
  program_id = shaders_link(vs, fs, gs);
}

glprog_t::glprog_t(const char *vs_src, const char *fs_src) {
  assert(vs_src != nullptr);
  assert(fs_src != nullptr);
  const int vs = shader_compile(vs_src, GL_VERTEX_SHADER);
  const int fs = shader_compile(fs_src, GL_FRAGMENT_SHADER);
  program_id = shaders_link(vs, fs);
}

void glprog_t::use() const { glUseProgram(program_id); }

int glprog_t::set(const std::string &key, const bool value) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform1i(location, (int) value);
  return 0;
}

int glprog_t::set(const std::string &key, const int value) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform1i(location, value);
  return 0;
}

int glprog_t::set(const std::string &key, const float value) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform1f(location, value);
  return 0;
}

int glprog_t::set(const std::string &key, const glm::vec2 &value) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform2fv(location, 1, &value[0]);
  return 0;
}

int glprog_t::set(const std::string &key,
                  const float x,
                  const float y) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform2f(location, x, y);
  return 0;
}

int glprog_t::set(const std::string &key, const glm::vec3 &value) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform3fv(location, 1, &value[0]);
  return 0;
}

int glprog_t::set(const std::string &key,
                  const float x,
                  const float y,
                  const float z) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform3f(location, x, y, z);
  return 0;
}

int glprog_t::set(const std::string &key, const glm::vec4 &value) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform4fv(location, 1, &value[0]);
  return 0;
}

int glprog_t::set(const std::string &key,
                  const float x,
                  const float y,
                  const float z,
                  const float w) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniform4f(location, x, y, z, w);
  return 0;
}

int glprog_t::set(const std::string &key, const glm::mat2 &mat) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
  return 0;
}

int glprog_t::set(const std::string &key, const glm::mat3 &mat) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
  return 0;
}

int glprog_t::set(const std::string &key, const glm::mat4 &mat) const {
  const auto location = glGetUniformLocation(program_id, key.c_str());
  if (location == -1) {
    return -1;
  }

  glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
  return 0;
}

/*****************************************************************************
 *                                 TEXTURE
 ****************************************************************************/

unsigned int load_texture(int img_width,
                          int img_height,
                          int img_channels,
                          const unsigned char *data) {
  unsigned int texture_id;

  glGenTextures(1, &texture_id);

  GLenum format;
  switch (img_channels) {
  case 1: format = GL_RED; break;
  case 3: format = GL_RGB; break;
  case 4: format = GL_RGBA; break;
  }

  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               format,
               img_width,
               img_height,
               0,
               format,
               GL_UNSIGNED_BYTE,
               data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return texture_id;
}

unsigned int load_texture(const std::string &texture_file,
                          int &img_width,
                          int &img_height,
                          int &img_channels) {
  unsigned char *data = stbi_load(texture_file.c_str(),
                                  &img_width,
                                  &img_height,
                                  &img_channels,
                                  0);
  if (!data) {
    printf("Failed to load texture at path [%s]!\n", texture_file.c_str());
  }

  unsigned int texture_id =
      load_texture(img_width, img_height, img_channels, data);
  stbi_image_free(data);

  return texture_id;
}

unsigned int load_texture(const std::string &texture_file) {
  int img_width = 0;
  int img_height = 0;
  int img_channels = 0;
  return load_texture(texture_file, img_width, img_height, img_channels);
}

/*****************************************************************************
 *                                 MESH
 ****************************************************************************/

glmesh_t::glmesh_t(const std::vector<glvertex_t> &vertices_,
                   const std::vector<unsigned int> &indices_,
                   const std::vector<gltexture_t> &textures_) {
  vertices = vertices_;
  indices = indices_;
  textures = textures_;
  glmesh_init(*this);
}

void glmesh_init(glmesh_t &mesh) {
  // Load data into vertex buffers
  // -- VAO
  glGenVertexArrays(1, &mesh.VAO);
  glBindVertexArray(mesh.VAO);

  // -- VBO
  glGenBuffers(1, &mesh.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  // A great thing about structs is that their memory layout is sequential for
  // all its items.  The effect is that we can simply pass a pointer to the
  // struct and it translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER,
               mesh.vertices.size() * sizeof(glvertex_t),
               &mesh.vertices[0],
               GL_STATIC_DRAW);

  // -- EBO
  glGenBuffers(1, &mesh.EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               mesh.indices.size() * sizeof(unsigned int),
               &mesh.indices[0],
               GL_STATIC_DRAW);

  // Set vertex attribute pointers
  // Vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(glvertex_t),
                        (void *) 0);

  // Vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(glvertex_t),
                        (void *) offsetof(glvertex_t, normal));

  // Vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(glvertex_t),
                        (void *) offsetof(glvertex_t, texcoords));

  // Vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(glvertex_t),
                        (void *) offsetof(glvertex_t, tangent));

  // Vertex bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(glvertex_t),
                        (void *) offsetof(glvertex_t, bitangent));

  glBindVertexArray(0);
}

void glmesh_draw(const glmesh_t &mesh, const glprog_t &program) {
  // bind appropriate textures
  unsigned int diffuse_counter = 1;
  unsigned int specular_counter = 1;
  unsigned int normal_counter = 1;
  unsigned int height_counter = 1;

  for (size_t i = 0; i < mesh.textures.size(); i++) {
    // Acitivate proper texture unit before binding
    glActiveTexture(GL_TEXTURE0 + i);

    // Retrieve texture number (the N in diffuse_textureN)
    std::string number;
    std::string name = mesh.textures[i].type;
    if (name == "texture_diffuse") {
      number = std::to_string(diffuse_counter++);
    } else if (name == "texture_specular") {
      // Transfer unsigned int to stream
      number = std::to_string(specular_counter++);
    } else if (name == "texture_normal") {
      // Transfer unsigned int to stream
      number = std::to_string(normal_counter++);
    } else if (name == "texture_height") {
      // Transfer unsigned int to stream
      number = std::to_string(height_counter++);
    }

    // Set the sampler to the correct texture unit and bind texture
    program.set((name + number), (int) i);
    // glUniform1i(glGetUniformLocation(program.program_id, (name +
    // number).c_str()), i);
    glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
  }

  // Draw mesh
  glBindVertexArray(mesh.VAO);
  glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // Set everything back to defaults once configured
  glActiveTexture(GL_TEXTURE0);
}

/*****************************************************************************
 *                                MODEL
 ****************************************************************************/

glmodel_t::glmodel_t(const std::string &path,
                     const std::string &vs,
                     const std::string &fs,
                     const bool gamma)
    : program{vs, fs}, gamma_correction(gamma) {
  glmodel_load(*this, path);
}

glmodel_t::glmodel_t(const std::string &path,
                     const char *vs,
                     const char *fs,
                     const bool gamma)
    : program{vs, fs}, gamma_correction(gamma) {
  glmodel_load(*this, path);
}

void glmodel_draw(glmodel_t &model, const glcamera_t &camera) {
  // Set projection and view
  model.program.use();
  model.program.set("projection", camera.projection());
  model.program.set("view", camera.view());
  model.program.set("model", model.T_SM);

  for (unsigned int i = 0; i < model.meshes.size(); i++) {
    glmesh_draw(model.meshes[i], model.program);
  }
}

void glmodel_load(glmodel_t &model, const std::string &path) {
  // Read
  Assimp::Importer importer;
  const auto options =
      aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
  const aiScene *scene = importer.ReadFile(path, options);

  // Check for errors
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    return;
  }

  // Retrieve the directory path of the filepath
  model.directory = path.substr(0, path.find_last_of('/'));

  // Process ASSIMP's root node recursively
  glmodel_process_node(model, scene->mRootNode, scene);
}

void glmodel_process_node(glmodel_t &model,
                          aiNode *node,
                          const aiScene *scene) {
  // Process each mesh located at the current node
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    // The node object only contains indices to index the actual objects in the
    // scene.  the scene contains all the data, node is just to keep stuff
    // organized (like relations between nodes).
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    model.meshes.push_back(glmodel_process_mesh(model, mesh, scene));
  }

  // After we've processed all of the meshes (if any) we then recursively
  // process each of the children nodes
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    glmodel_process_node(model, node->mChildren[i], scene);
  }
}

glmesh_t glmodel_process_mesh(glmodel_t &model,
                              aiMesh *mesh,
                              const aiScene *scene) {
  // Data to fill
  std::vector<glvertex_t> vertices;
  std::vector<unsigned int> indices;
  std::vector<gltexture_t> textures;

  // Walk through each of the mesh's vertices
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    glvertex_t vertex;
    glm::vec3 vector;
    // Placeholder vector since assimp uses its own vector class that doesn't
    // directly convert to glm's vec3 class so we transfer the data to this
    // placeholder glm::vec3 first.

    // Positions
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position = vector;

    // Normals
    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.normal = vector;

    // Texture coordinates
    if (mesh->mTextureCoords[0]) {
      // Does the mesh contain texture coordinates? A vertex can contain up to
      // 8 different texture coordinates. We thus make the assumption that we
      // won't use models where a vertex can have multiple texture coordinates
      // so we always take the first set (0).
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.texcoords = vec;
    } else {
      vertex.texcoords = glm::vec2(0.0f, 0.0f);
    }

    // Tangent
    vector.x = mesh->mTangents[i].x;
    vector.y = mesh->mTangents[i].y;
    vector.z = mesh->mTangents[i].z;
    vertex.tangent = vector;

    // Bi-tangent
    vector.x = mesh->mBitangents[i].x;
    vector.y = mesh->mBitangents[i].y;
    vector.z = mesh->mBitangents[i].z;
    vertex.bitangent = vector;
    vertices.push_back(vertex);
  }

  // Now walk through each of the mesh's faces (a face is a mesh its triangle)
  // and retrieve the corresponding vertex indices.
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];

    // Retrieve all indices of the face and store them in the indices vector
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  // Process materials
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  // we assume a convention for sampler names in the shaders. Each diffuse
  // texture should be named as 'texture_diffuseN' where N is a sequential
  // number ranging from 1 to MAX_SAMPLER_NUMBER.  Same applies to other
  // texture as the following list summarizes:
  // - diffuse: texture_diffuseN
  // - specular: texture_specularN
  // - normal: texture_normalN

  // clang-format off
  // 1. Diffuse maps
  const auto diffuse_maps = glmodel_load_textures(
    model,
    material,
    aiTextureType_DIFFUSE,
    "texture_diffuse"
  );
  textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

  // 2. Specular maps
  const auto specularMaps = glmodel_load_textures(
    model,
    material,
    aiTextureType_SPECULAR,
    "texture_specular"
  );
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  // 3. Normal maps
  const auto normalMaps = glmodel_load_textures(
    model,
    material,
    aiTextureType_HEIGHT,
    "texture_normal"
  );
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  // 4. Height maps
  const auto heightMaps = glmodel_load_textures(
    model,
    material,
    aiTextureType_AMBIENT,
    "texture_height"
  );
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
  // clang-format on

  // Return a mesh object created from the extracted mesh data
  return glmesh_t{vertices, indices, textures};
}

// checks all material textures of a given type and loads the textures if
// they're not loaded yet. the required info is returned as a Texture struct.
std::vector<gltexture_t> glmodel_load_textures(glmodel_t &model,
                                               aiMaterial *mat,
                                               aiTextureType type,
                                               std::string typeName) {
  std::vector<gltexture_t> textures;

  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);

    // check if texture was loaded before and if so, continue to next
    // iteration: skip loading a new texture
    bool skip = false;
    for (unsigned int j = 0; j < model.textures_loaded.size(); j++) {
      if (std::strcmp(model.textures_loaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(model.textures_loaded[j]);
        skip = true;
        // a texture with the same filepath has already been
        // loaded, continue to next one. (optimization)
        break;
      }
    }

    if (!skip) { // if texture hasn't been loaded already, load it
      gltexture_t texture;
      // const std::string texture_path = model.directory + '/' + str.C_Str();
      texture.id = texture_from_file(model.directory, str.C_Str());
      // texture.id = load_texture(texture_path);
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      model.textures_loaded.push_back(texture);
      // store it as texture loaded for entire model, to ensure we won't
      // unnecesery load duplicate textures.
    }
  }

  return textures;
}

unsigned int texture_from_file(const std::string &dir,
                               const char *fp,
                               bool /*gamma*/) {
  const std::string filename = dir + '/' + std::string(fp);
  unsigned int texture_id;
  glGenTextures(1, &texture_id);

  int width, height, nb_components;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nb_components, 0);

  if (data) {
    GLenum format;
    switch (nb_components) {
    case 1: format = GL_RED; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 format,
                 width,
                 height,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    LOG_ERROR("Texture failed to load at path: %s", fp);
  }

  stbi_image_free(data);
  return texture_id;
}

/*****************************************************************************
 *                                DRAW
 ****************************************************************************/

globj_t::globj_t(const char *vs, const char *fs) : program_{vs, fs} {}

void globj_t::pos(const glm::vec3 &pos) { T_SM_ = glm::translate(T_SM_, pos); }

glm::vec3 globj_t::pos() { return glm::vec3(T_SM_[3]); }

glm::mat3 globj_t::rot() {
  glm::vec3 col_0(T_SM_[0]);
  glm::vec3 col_1(T_SM_[1]);
  glm::vec3 col_2(T_SM_[2]);
  glm::mat3 R(col_0, col_1, col_2);
  return R;
}

glcf_t::glcf_t() : globj_t{shaders::glcf_vs, shaders::glcf_fs} {
  // Form the camera fov frame
  float hfov = fov_ / 2.0f;
  float z = scale_;
  float hwidth = z * glm::tan(hfov);
  const glm::vec3 lb{-hwidth, hwidth, z};  // Left bottom
  const glm::vec3 lt{-hwidth, -hwidth, z}; // Left top
  const glm::vec3 rt{hwidth, -hwidth, z};  // Right top
  const glm::vec3 rb{hwidth, hwidth, z};   // Right bottom

  // Rectangle frame
  // clang-format off
  static const GLfloat vertices[] = {
    // -- Left bottom to left top
    lb.x, lb.y, lb.z, lt.x, lt.y, lt.z,
    // -- Left top to right top
    lt.x, lt.y, lt.z, rt.x, rt.y, rt.z,
    // -- Right top to right bottom
    rt.x, rt.y, rt.z, rb.x, rb.y, rb.z,
    // -- Right bottom to left bottom
    rb.x, rb.y, rb.z, lb.x, lb.y, lb.z,
    // Rectangle frame to origin
    // -- Origin to left bottom
    0.0f, 0.0f, 0.0f, lb.x, lb.y, lb.z,
    // -- Origin to left top
    0.0f, 0.0f, 0.0f, lt.x, lt.y, lt.z,
    // -- Origin to right top
    0.0f, 0.0f, 0.0f, rt.x, rt.y, rt.z,
    // -- Origin to right bottom
    0.0f, 0.0f, 0.0f, rb.x, rb.y, rb.z
  };
  // clang-format on
  const size_t nb_lines = 8;
  const size_t nb_vertices = nb_lines * 2;
  const size_t buffer_size = sizeof(GLfloat) * nb_vertices * 3;

  // VAO
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // VBO
  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        3 * sizeof(float),
                        (void *) 0);
  glEnableVertexAttribArray(0);

  // Clean up
  glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
  glBindVertexArray(0);             // Unbind VAO
}

glcf_t::~glcf_t() {
  glDeleteVertexArrays(1, &VAO_);
  glDeleteBuffers(1, &VBO_);
}

void glcf_t::draw(const glcamera_t &camera) {
  program_.use();
  program_.set("projection", camera.projection());
  program_.set("view", camera.view());
  program_.set("model", T_SM_);

  // Store original line width
  float original_line_width = 0.0f;
  glGetFloatv(GL_LINE_WIDTH, &original_line_width);

  // Set line width
  glLineWidth(line_width_);

  // Draw frame
  const size_t nb_lines = 8;
  const size_t nb_vertices = nb_lines * 2;
  glBindVertexArray(VAO_);
  glDrawArrays(GL_LINES, 0, nb_vertices);
  glBindVertexArray(0); // Unbind VAO

  // Restore original line width
  glLineWidth(original_line_width);
}

glcube_t::glcube_t() : glcube_t{0.5} {}

glcube_t::glcube_t(const float cube_size)
	: globj_t{shaders::glcube_vs, shaders::glcube_fs},
		cube_size_{cube_size} {
  // Vertices
  // clang-format off
  const float r = color_.x;
  const float g = color_.y;
  const float b = color_.z;
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
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // VBO
  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
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

glcube_t::~glcube_t() {
  glDeleteVertexArrays(1, &VAO_);
  glDeleteBuffers(1, &VBO_);
}

void glcube_t::draw(const glcamera_t &camera) {
  program_.use();
  program_.set("projection", camera.projection());
  program_.set("view", camera.view());
  program_.set("model", T_SM_);

  // 12 x 3 indices starting at 0 -> 12 triangles -> 6 squares
  glBindVertexArray(VAO_);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0); // Unbind VAO
}

glvoxels_t::glvoxels_t(const float voxel_size, const size_t nb_voxels)
    : globj_t{shaders::glcube_vs, shaders::glcube_fs} {
  // Vertices
  // clang-format off
  glm::vec3 color{0.9, 0.4, 0.2};
  const float r = color.x;
  const float g = color.y;
  const float b = color.z;

  for (size_t i = 0; i < nb_voxels; i++) {
    static const GLfloat voxel_vertices[18 * 12] = {
      // Triangle 1
      -voxel_size, -voxel_size, -voxel_size, r, g, b,
      -voxel_size, -voxel_size, voxel_size, r, g, b,
      -voxel_size, voxel_size, voxel_size, r, g, b,
      // Triangle 2
      voxel_size, voxel_size, -voxel_size, r, g, b,
      -voxel_size, -voxel_size, -voxel_size, r, g, b,
      -voxel_size, voxel_size, -voxel_size, r, g, b,
      // Triangle 3
      voxel_size, -voxel_size, voxel_size, r, g, b,
      -voxel_size, -voxel_size, -voxel_size, r, g, b,
      voxel_size, -voxel_size, -voxel_size, r, g, b,
      // Triangle 4
      voxel_size, voxel_size, -voxel_size, r, g, b,
      voxel_size, -voxel_size, -voxel_size, r, g, b,
      -voxel_size, -voxel_size, -voxel_size, r, g, b,
      // Triangle 5
      -voxel_size, -voxel_size, -voxel_size, r, g, b,
      -voxel_size, voxel_size, voxel_size, r, g, b,
      -voxel_size, voxel_size, -voxel_size, r, g, b,
      // Triangle 6
      voxel_size, -voxel_size, voxel_size, r, g, b,
      -voxel_size, -voxel_size, voxel_size, r, g, b,
      -voxel_size, -voxel_size, -voxel_size, r, g, b,
      // Triangle 7
      -voxel_size, voxel_size, voxel_size, r, g, b,
      -voxel_size, -voxel_size, voxel_size, r, g, b,
      voxel_size, -voxel_size, voxel_size, r, g, b,
      // Triangle 8
      voxel_size, voxel_size, voxel_size, r, g, b,
      voxel_size, -voxel_size, -voxel_size, r, g, b,
      voxel_size, voxel_size, -voxel_size, r, g, b,
      // Triangle 9
      voxel_size, -voxel_size, -voxel_size, r, g, b,
      voxel_size, voxel_size, voxel_size, r, g, b,
      voxel_size, -voxel_size, voxel_size, r, g, b,
      // Triangle 10
      voxel_size, voxel_size, voxel_size, r, g, b,
      voxel_size, voxel_size, -voxel_size, r, g, b,
      -voxel_size, voxel_size, -voxel_size, r, g, b,
      // Triangle 11
      voxel_size, voxel_size, voxel_size, r, g, b,
      -voxel_size, voxel_size, -voxel_size, r, g, b,
      -voxel_size, voxel_size, voxel_size, r, g, b,
      // Triangle 12
      voxel_size, voxel_size, voxel_size, r, g, b,
      -voxel_size, voxel_size, voxel_size, r, g, b,
      voxel_size, -voxel_size, voxel_size, r, g, b
    };

    for (size_t j = 0; j < (18 * 12); j++) {
      vertices.push_back(voxel_vertices[j]);
    }
  }

  const size_t nb_triangles = 12 * nb_voxels;
  const size_t vertices_per_triangle = 3;
  const size_t nb_vertices = vertices_per_triangle * nb_triangles;
  const size_t vertex_buffer_size = sizeof(float) * 6 * nb_vertices;
  // clang-format on

  // VAO
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // VBO
  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER,
               vertex_buffer_size,
               vertices.data(),
               GL_STATIC_DRAW);
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

glvoxels_t::~glvoxels_t() {}

void glvoxels_t::draw(const glcamera_t &camera) {
  program_.use();
  program_.set("projection", camera.projection());
  program_.set("view", camera.view());
  program_.set("model", T_SM_);

  // 12 x 3 indices starting at 0 -> 12 triangles -> 6 squares
  glBindVertexArray(VAO_);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0); // Unbind VAO
}


glframe_t::glframe_t() : globj_t{shaders::glframe_vs, shaders::glframe_fs} {
  // Vertices
  // clang-format off
  static const GLfloat vertices[] = {
    // Line 1 : x-axis + color
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    // Line 2 : y-axis + color
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // Line 3 : z-axis + color
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
  };
  const size_t nb_vertices = 6;
  const size_t buffer_size = sizeof(GLfloat) * 6 * nb_vertices;
  // clang-format on

  // VAO
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // VBO
  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);
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

glframe_t::~glframe_t() {
  glDeleteVertexArrays(1, &VAO_);
  glDeleteBuffers(1, &VBO_);
}

void glframe_t::draw(const glcamera_t &camera) {
  program_.use();
  program_.set("projection", camera.projection());
  program_.set("view", camera.view());
  program_.set("model", T_SM_);

  // Store original line width
  float original_line_width = 0.0f;
  glGetFloatv(GL_LINE_WIDTH, &original_line_width);

  // Set line width
  glLineWidth(line_width_);

  // Draw frame
  glBindVertexArray(VAO_);
  glDrawArrays(GL_LINES, 0, 6);
  glBindVertexArray(0); // Unbind VAO

  // Restore original line width
  glLineWidth(original_line_width);
}

static GLfloat *glgrid_create_vertices(int grid_size) {
  // Allocate memory for vertices
  int nb_lines = (grid_size + 1) * 2;
  int nb_vertices = nb_lines * 2;
  const size_t buffer_size = sizeof(GLfloat) * nb_vertices * 3;
  GLfloat *vertices = (GLfloat *) malloc(buffer_size);

  // Setup
  const float min_x = -1.0f * (float) grid_size / 2.0f;
  const float max_x = (float) grid_size / 2.0f;
  const float min_z = -1.0f * (float) grid_size / 2.0f;
  const float max_z = (float) grid_size / 2.0f;

  // Row vertices
  float z = min_z;
  int vert_idx = 0;
  for (int i = 0; i < ((grid_size + 1) * 2); i++) {
    if ((i % 2) == 0) {
      vertices[(vert_idx * 3)] = min_x;
      vertices[(vert_idx * 3) + 1] = 0.0f;
      vertices[(vert_idx * 3) + 2] = z;
    } else {
      vertices[(vert_idx * 3)] = max_x;
      vertices[(vert_idx * 3) + 1] = 0.0f;
      vertices[(vert_idx * 3) + 2] = z;
      z += 1.0f;
    }
    vert_idx++;
  }

  // Column vertices
  float x = min_x;
  for (int j = 0; j < ((grid_size + 1) * 2); j++) {
    if ((j % 2) == 0) {
      vertices[(vert_idx * 3)] = x;
      vertices[(vert_idx * 3) + 1] = 0.0f;
      vertices[(vert_idx * 3) + 2] = min_z;
    } else {
      vertices[(vert_idx * 3)] = x;
      vertices[(vert_idx * 3) + 1] = 0.0f;
      vertices[(vert_idx * 3) + 2] = max_z;
      x += 1.0f;
    }
    vert_idx++;
  }

  return vertices;
}

glgrid_t::glgrid_t() : globj_t{shaders::glgrid_vs, shaders::glgrid_fs} {
  // Create vertices
  const int nb_lines = (grid_size_ + 1) * 2;
  const int nb_vertices = nb_lines * 2;
  GLfloat *vertices = glgrid_create_vertices(grid_size_);
  const size_t buffer_size = sizeof(GLfloat) * nb_vertices * 3;

  // VAO
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // VBO
  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        3 * sizeof(float),
                        (void *) 0);
  glEnableVertexAttribArray(0);

  // Clean up
  glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
  glBindVertexArray(0);             // Unbind VAO
  free(vertices);
}

glgrid_t::~glgrid_t() {
  glDeleteVertexArrays(1, &VAO_);
  glDeleteBuffers(1, &VBO_);
}

void glgrid_t::draw(const glcamera_t &camera) {
  program_.use();
  program_.set("projection", camera.projection());
  program_.set("view", camera.view());
  program_.set("model", T_SM_);

  const int nb_lines = (grid_size_ + 1) * 2;
  const int nb_vertices = nb_lines * 2;

  glBindVertexArray(VAO_);
  glDrawArrays(GL_LINES, 0, nb_vertices);
  glBindVertexArray(0); // Unbind VAO
}

glplane_t::glplane_t(const std::string &image_path)
    : globj_t{shaders::glplane_vs, shaders::glplane_fs}, image_path_{
                                                             image_path} {
  // clang-format off
  const float vertices[] = {
    // Positions         // Colors           // Texture coords
    0.5f,  0.5f,  0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  // Top right
    0.5f, -0.5f,  0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  // Bottom right
    -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  // Bottom left
    -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f   // Top left
  };
  const unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
  };
  // clang-format on

  // VAO
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // VBO
  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // -- Position attribute
  const void *pos_offset = (void *) 0;
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        8 * sizeof(float),
                        pos_offset);
  glEnableVertexAttribArray(0);
  // -- Color attribute
  void *color_offset = (void *) (3 * sizeof(float));
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        8 * sizeof(float),
                        color_offset);
  glEnableVertexAttribArray(1);
  // -- Texture coord attribute
  void *texture_offset = (void *) (6 * sizeof(float));
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        8 * sizeof(float),
                        texture_offset);
  glEnableVertexAttribArray(2);

  // EBO
  glGenBuffers(1, &EBO_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(indices),
               indices,
               GL_STATIC_DRAW);

  // FBO
  glGenFramebuffers(1, &FBO_);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO_);

  // Load and create a texture
  texture_ = load_texture(image_path_, img_width_, img_height_, img_channels_);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,
                         texture_,
                         0);

  // Render buffer
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER,
                        GL_DEPTH24_STENCIL8,
                        img_width_,
                        img_height_);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                            GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER,
                            rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Framebuffer is not complete!\n");
  }

  // Clean up
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);     // Unbind VBO
  glBindVertexArray(0);                 // Unbind VAO
}

void glplane_t::draw(const glcamera_t &camera) {
  UNUSED(camera);
  // glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
  // glEnable(GL_DEPTH_TEST);
  // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  program_.use();
  // program.set("projection", camera.projection());
  // program.set("view", camera.view());
  // program.set("model", T_SM);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0); // Unbind VAO

  // glDisable(GL_DEPTH_TEST);
  // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*****************************************************************************
 *                                   GUI
 ****************************************************************************/


gui_t::gui_t(const std::string &title_, const int width_, const int height_)
    : title{title_}, width{width_}, height{height_} {
  // Set GLFW error callback
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    FATAL("Failed to initialize GLFW!");
  }

  // Decide GL+GLSL versions
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create window with graphics context
  gui = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (gui == NULL) {
    FATAL("Failed to create a GLFW window!");
  }
  glfwMakeContextCurrent(gui);
  glfwSetWindowAspectRatio(gui, width, height);
  glfwSwapInterval(1); // Enable vsync

  // Event handlers
	glfwWaitEventsTimeout(0.1);
  glfwSetWindowUserPointer(gui, this);
  // -- Mouse
  glfwSetCursorPosCallback(gui, mouse_cursor_cb);
	glfwSetMouseButtonCallback(gui, mouse_button_cb);
  glfwSetScrollCallback(gui, mouse_scroll_cb);
  // -- Window
  glfwSetFramebufferSizeCallback(gui, window_cb);

  // Initialize OpenGL loader
  bool err = gladLoadGL() == 0;
  if (err) {
    FATAL("Failed to initialize OpenGL loader!");
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  // ImGuiIO& io = ImGui::GetIO();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(gui, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

gui_t::gui_t(const std::string &title)
	: gui_t{title, 1024, 768} {}

gui_t::~gui_t() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(gui);
  glfwTerminate();
}

bool gui_t::ok() {
  float time_now = glfwGetTime();
  dt = time_now - time_last;
  time_last = time_now;

	return !glfwWindowShouldClose(gui) && keep_running;
}

void gui_t::poll() {
  glfwPollEvents();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  glfwGetWindowSize(gui, &width, &height);
  keyboard_cb(gui);
}

void gui_t::clear() {
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
}

void gui_t::render(const bool clear_gui) {
  if (clear_gui) {
    clear();
  }
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // glfwMakeContextCurrent(gui_);
	glEnable(GL_CULL_FACE);
  glfwSwapBuffers(gui);
}

void gui_t::loop(std::function<int()> cb) {
  while (ok()) {
    poll();
    clear();

    if (cb() != 0) {
      keep_running = false;
    }

    render();
  }
}

void gui_t::error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void gui_t::window_cb(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void gui_t::mouse_cursor_cb(GLFWwindow *window, double x, double y) {
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

void gui_t::mouse_button_cb(GLFWwindow *window,
                            int btn,
                            int action,
                            int mods) {
  gui_t *gui = reinterpret_cast<gui_t *>(glfwGetWindowUserPointer(window));
  if (btn == GLFW_MOUSE_BUTTON_LEFT) {
    gui->left_click = (action == GLFW_PRESS) ? true : false;
  } else if (btn == GLFW_MOUSE_BUTTON_RIGHT) {
    gui->right_click = (action == GLFW_PRESS) ? true : false;
  }
}

void gui_t::mouse_scroll_cb(GLFWwindow *window, double dx, double dy) {
  gui_t *gui = reinterpret_cast<gui_t *>(glfwGetWindowUserPointer(window));
  gui->camera.zoom(dy);
}

void gui_t::keyboard_cb(GLFWwindow *window) {
  gui_t *gui = reinterpret_cast<gui_t *>(glfwGetWindowUserPointer(window));

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

/*****************************************************************************
 *                               GUI IMSHOW
 ****************************************************************************/

gui_imshow_t::gui_imshow_t(const std::string &title) : title_{title} {}

gui_imshow_t::gui_imshow_t(const std::string &title,
                           const std::string &img_path)
    : title_{title}, img_path_{img_path} {
  unsigned char *data =
      stbi_load(img_path.c_str(), &img_width_, &img_height_, &img_channels_, 0);
  if (!data) {
    FATAL("Failed to load texture at path [%s]!\n", img_path.c_str());
  }
  init(title, img_width_, img_height_, img_channels_, data);
  stbi_image_free(data);
}

gui_imshow_t::gui_imshow_t(const std::string &title,
                           const int img_width,
                           const int img_height,
                           const int img_channels,
                           const unsigned char *data) {
  init(title, img_width, img_height, img_channels, data);
}

bool gui_imshow_t::ok() { return ok_; }

void gui_imshow_t::init(const std::string &title,
                        const int img_width,
                        const int img_height,
                        const int img_channels,
                        const unsigned char *data) {
  title_ = title;
  img_width_ = img_width;
  img_height_ = img_height;
  img_channels_ = img_channels;

  // FBO
  glGenFramebuffers(1, &FBO_);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO_);

  // Load and create a texture
  img_id_ = load_texture(img_width_, img_height_, img_channels_, data);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,
                         img_id_,
                         0);

  // Render buffer
  glGenRenderbuffers(1, &RBO_);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO_);
  glRenderbufferStorage(GL_RENDERBUFFER,
                        GL_DEPTH24_STENCIL8,
                        img_width,
                        img_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                            GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER,
                            RBO_);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    FATAL("Framebuffer is not complete!\n");
  }

  // Clean up
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO
  ok_ = true;
}

void gui_imshow_t::update(void *pixels) {
  GLenum img_format;
  switch (img_channels_) {
  case 1: img_format = GL_RED; break;
  case 3: img_format = GL_RGB; break;
  case 4: img_format = GL_RGBA; break;
  }

  glBindTexture(GL_TEXTURE_2D, img_id_);
  glTexSubImage2D(GL_TEXTURE_2D,
                  0,
                  0,
                  0,
                  img_width_,
                  img_height_,
                  img_format,
                  GL_UNSIGNED_BYTE,
                  pixels);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void gui_imshow_t::show() {
  // Set window alpha
  float alpha = 2.0f;
  ImGui::SetNextWindowBgAlpha(alpha);

  // Set window size
  ImVec2 win_size(img_width_ + 15, img_height_ + 35);
  ImGui::SetNextWindowSize(win_size);

  // Begin window
  bool open = false;
  const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;
  ImGui::Begin(title_.c_str(), &open, flags);

  // Add image
  const auto start = ImGui::GetCursorScreenPos();
  const auto end_x = start.x + img_width_;
  const auto end_y = start.y + img_height_;
  const auto end = ImVec2(end_x, end_y);
  ImGui::GetWindowDrawList()->AddImage((void *) (intptr_t) img_id_,
                                       ImVec2(start.x, start.y),
                                       end);

  // End window
  ImGui::End();
}

void gui_imshow_t::show(const int img_width,
                        const int img_height,
                        const int img_channels,
                        const unsigned char *data) {
  if (ok_ == false) {
    init(title_, img_width, img_height, img_channels, data);
  }

  update((void *) data);
  show();
}

} // namespace proto

int main() {
  show::gui_t gui{"Show"};
  show::glgrid_t grid;
	show::glcube_t cube;

  gui.loop([&]() {
		grid.draw(gui.camera);
		cube.draw(gui.camera);
    return 0;
	});

	return 0;
}
