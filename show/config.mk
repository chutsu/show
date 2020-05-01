CXX= g++

CFLAGS= \
	-I../deps/ \
	-I../deps/stb \
	-I../deps/imgui -I../deps/imgui/examples/example_glfw_opengl3

LIBS= \
	-L../deps/glfw/build/src -lglfw3 \
	-L../deps/assimp/build -lassimp \
	-L../deps/glad/ -lglad -ldl \
	-L../deps/imgui/ -limgui \
	-lpthread
