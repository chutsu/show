include config.mk
.PHONY: default bin clean examples

SHOW_APP=$(BIN_DIR)/show

EXAMPLE-HELLO_WORLD=$(BIN_DIR)/examples-hello_world
EXAMPLE-RECTANGLE=$(BIN_DIR)/examples-rectangle
EXAMPLE-SHADER=$(BIN_DIR)/examples-shader
EXAMPLE-TEXTURE=$(BIN_DIR)/examples-texture

EXAMPLES=$(EXAMPLE-HELLO_WORLD) \
				 $(EXAMPLE-RECTANGLE) \
				 $(EXAMPLE-SHADER) \
				 $(EXAMPLE-TEXTURE)

default: bin $(SHOW_APP) $(EXAMPLES)

bin:
	@mkdir -p $(BIN_DIR)
	@cd $(BIN_DIR) && ln -s ../assets .

clean:
	@rm -rf $(BIN_DIR)

# SHOW APP
$(SHOW_APP): show.cpp show.hpp
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
