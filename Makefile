include config.mk
.PHONY: default bin clean examples

SHOW_APP=$(BIN_DIR)/show
SHOW_TEST=$(BIN_DIR)/test_show

EXAMPLE-HELLO_WORLD=$(BIN_DIR)/examples-hello_world
EXAMPLE-RECTANGLE=$(BIN_DIR)/examples-rectangle
EXAMPLE-SHADER=$(BIN_DIR)/examples-shader
EXAMPLE-TEXTURE=$(BIN_DIR)/examples-texture
EXAMPLE-CAMERA=$(BIN_DIR)/examples-camera
EXAMPLE-IMSHOW=$(BIN_DIR)/examples-imshow

EXAMPLES=$(EXAMPLE-HELLO_WORLD) \
				 $(EXAMPLE-RECTANGLE) \
				 $(EXAMPLE-SHADER) \
				 $(EXAMPLE-TEXTURE) \
				 $(EXAMPLE-CAMERA) \
				 $(EXAMPLE-IMSHOW)

default: bin $(SHOW_APP) $(EXAMPLES)

bin:
	@mkdir -p $(BIN_DIR)
	@cd $(BIN_DIR) && ln -fs ../assets .

clean:
	@rm -rf $(BIN_DIR)

# SHOW APP
$(SHOW_APP): show/show.cpp show/show.hpp
	@$(BUILD_BIN)

$(SHOW_TEST): show/test_show.cpp
	@$(BUILD_BIN)

# EXAMPLES
$(EXAMPLE-HELLO_WORLD): examples/hello_world.cpp
	@$(BUILD_BIN)

$(EXAMPLE-RECTANGLE): examples/rectangle.cpp
	@$(BUILD_BIN)

$(EXAMPLE-SHADER): examples/shader.cpp
	@$(BUILD_BIN)

$(EXAMPLE-TEXTURE): examples/texture.cpp
	@$(BUILD_BIN)

$(EXAMPLE-CAMERA): examples/camera.cpp
	@$(BUILD_BIN)

$(EXAMPLE-IMSHOW): examples/imshow.cpp
	@$(BUILD_BIN)
