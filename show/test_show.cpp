#include "munit.hpp"
#include "show.hpp"

int test_gui_imshow() {
  show::gui_t gui{"Show"};
  show::gui_imshow_t imshow{"Image", "assets/container.jpg"};

  gui.loop([&]() {
		imshow.draw(gui.camera);
    return 0;
	});

  return 0;
}

void test_suite() {
  MU_ADD_TEST(test_gui_imshow);
}

MU_RUN_TESTS(test_suite);
