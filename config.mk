BIN_DIR=$(PWD)/bin
DEP_DIR=$(PWD)/deps

CXX=g++

CXXFLAGS=\
	-I$(DEP_DIR)/ \
	-I$(DEP_DIR)/stb \
	-I$(DEP_DIR)/imgui -I../deps/imgui/examples/example_glfw_opengl3

LIBS=\
	-L$(BIN_DIR) -lshow \
	-L$(DEP_DIR)/glfw/build/src -lglfw3 \
	-L$(DEP_DIR)/assimp/build -lassimp \
	-L$(DEP_DIR)/glad/ -lglad -ldl \
	-L$(DEP_DIR)/imgui/ -limgui \
	-lpthread

AR = ar
ARFLAGS = rvs

BUILD_LIB=\
	echo "[AR]: $@"; \
	$(CXX) $(CXXFLAGS) -c $< -o $@.o; \
	$(AR) $(ARFLAGS) $@ $@.o; \
	rm $@.o

BUILD_BIN=\
  echo "[CXX]: $(subst $(BIN_DIR)/,,$<)"; \
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
