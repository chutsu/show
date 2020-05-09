GLAD_SRC_DIR=$(PWD)/glad/src
GLAD_INC_DIR=$(PWD)/glad/include
GLAD_BLD_DIR=$(PWD)/glad/build
LIBGLAD=$(GLAD_BLD_DIR)/libglad.a
ASSIMP=$(PWD)/assimp
GLFW=$(PWD)/glfw
GLM=$(PWD)/glm
STB=$(PWD)/stb
IMGUI=$(PWD)/imgui
OCTOMAP=$(PWD)/octomap

all: $(LIBGLAD) $(ASSIMP) $(GLFW) $(GLM) $(STB) $(IMGUI) $(OCTOMAP)

clean:
	@rm -rf build
	@rm -rf glad

glad:
	@pip install glad
	@mkdir -p glad
	@mkdir -p glad/build
	@glad --quiet --generator c --out glad

$(LIBGLAD): glad
	@g++ -I$(GLAD_INC_DIR) \
		-c $(GLAD_SRC_DIR)/glad.c \
		-o $(GLAD_BLD_DIR)/glad.o; \
	ar rs $(GLAD_BLD_DIR)/libglad.a $(GLAD_BLD_DIR)/glad.o

$(ASSIMP):
	@git clone https://github.com/assimp/assimp \
		&& cd assimp && mkdir build && cd build \
		&& cmake .. -DCMAKE_BUILD_TYPE=Release \
		&& make -j2

$(GLFW):
	@git clone https://github.com/glfw/glfw \
		&& cd glfw && mkdir build && cd build \
		&& cmake .. -DCMAKE_BUILD_TYPE=Release -D_GLFW_X11=1 \
		&& make -j2

$(GLM):
	@git clone https://github.com/g-truc/glm \
		&& cd glm && mkdir -p build && cd build \
		&& cmake -DCMAKE_BUILD_TYPE=RELEASE .. \
		&& make -j2

$(STB):
	@git clone https://github.com/nothings/stb

$(IMGUI):
	@git clone https://github.com/ocornut/imgui \
		&& cd imgui \
		&& g++ -I${PWD}/imgui \
			-DIMGUI_IMPL_OPENGL_LOADER_GLAD \
			-c imgui.cpp \
			imgui_demo.cpp \
			imgui_draw.cpp \
			imgui_widgets.cpp \
			examples/imgui_impl_glfw.cpp \
			examples/imgui_impl_opengl3.cpp \
		&& ar rs libimgui.a \
			imgui.o \
			imgui_demo.o \
			imgui_draw.o \
			imgui_widgets.o \
			imgui_impl_glfw.o \
			imgui_impl_opengl3.o

$(OCTOMAP):
	@git clone https://github.com/OctoMap/octomap \
		&& cd octomap && mkdir -p build && cd build \
		&& cmake -DCMAKE_BUILD_TYPE=RELEASE .. \
		&& make -j2
