BIN_DIR=$(PWD)/bin
DEP_DIR=$(PWD)/deps

CXX=g++

CFLAGS=\
	-I$(DEP_DIR)/ \
	-I$(DEP_DIR)/stb \
	-I$(DEP_DIR)/imgui -I../deps/imgui/examples/example_glfw_opengl3

LIBS=\
	-L$(DEP_DIR)/glfw/build/src -lglfw3 \
	-L$(DEP_DIR)/assimp/build -lassimp \
	-L$(DEP_DIR)/glad/ -lglad -ldl \
	-L$(DEP_DIR)/imgui/ -limgui \
	-lpthread

BUILD_BIN=\
  echo "[BIN]:$(subst $(BIN_DIR)/, , $@)"; \
	$(CXX) $(CFLAGS) $< -o $@ $(LIBS)
