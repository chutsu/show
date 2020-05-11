#include "show.hpp"

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
