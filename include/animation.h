#include <cassette/cgui-window.h>
#include <cassette/cgui.h>
#include <cairo/cairo.h>

typedef struct {
  cgui_window *window;
  cgui_window *parent_window;
  cgui_grid *grid;
  cairo_t *cairo_context;
  cairo_surface_t *cairo_surface;
} animation_t;

enum animation_mode_t { ANIMATION_SUCCESS, ANIMATION_FAILURE };

void init_animation(animation_t *animation, cgui_window *parent_window);
void destroy_animation(animation_t *animation);
void activate_animation(animation_t *animation);
void do_animation(animation_t *animation, enum animation_mode_t mode);
