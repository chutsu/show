default: show

show: show.cpp show.hpp
	@g++ -c show.cpp\
		-I../deps/ \
		-L../deps/glfw/build -lglfw \
		-I../deps/stb \
		-L../deps/assimp/build -lassimp \
		-L../deps/glad/ -lglad -ldl \
		-I../deps/imgui -I../deps/imgui/examples/example_glfw_opengl3 \
		-L../deps/imgui/ -limgui
	@g++ show.o \
		-I../deps/ \
		-L../deps/glfw/build -lglfw \
		-I../deps/stb \
		-L../deps/assimp/build -lassimp \
		-L../deps/glad/ -lglad -ldl \
		-I../deps/imgui -I../deps/imgui/examples/example_glfw_opengl3 \
		-L../deps/imgui/ -limgui \
		-o show
