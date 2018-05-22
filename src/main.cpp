#include <GL/gl3w.h>
#include "simgl.hpp"

#include <nuklear/nuklear.h>

int filter_none(const struct nk_text_edit *e, nk_rune unicode) {
	return nk_true;
}

int main(int argc, char *argv[]) {
	
	sgl::window_context wctx;
	wctx.resizable = false;
	wctx.width = wctx.height = 740;
	sgl::window window(wctx);

	window.on_resize([](sgl::window &window, int w, int h) { glViewport(0, 0, w, h); });
	
	window.on_update([&](sgl::window &, nk_context *ctx) {
		static float r = 0.33f, g = 0.33f, b = 0.33f;
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

	});

	return 0;
}

