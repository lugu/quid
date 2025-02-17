
#include <cairo/cairo.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "animation.h"

/*
 * Cassette doesn't let us provide private data associated with a window
 * so we have to rely on global to retreive the state of our application
 */
static animation_t *_animation;

static void udpate_animation(animation_t *animation) {
}

static void on_draw(cgui_window *window, unsigned long delay) {
  udpate_animation(_animation);
  cgui_window_redraw_delayed(window, 20 /* milliseconds */);
}

void init_animation(animation_t *animation, cgui_window *parent_window) {
  _animation = animation;
  animation->parent_window = parent_window;
  animation->window = cgui_window_create();
  animation->cairo_context = cgui_window_cairo_drawable(animation->window);
  animation->cairo_surface = cgui_window_cairo_surface(animation->window);
  animation->grid = cgui_grid_create(1, 1);

  cgui_grid_resize_col(animation->grid, 0, 20);
  cgui_grid_resize_row(animation->grid, 0, 20);

  cgui_window_push_grid(animation->window, animation->grid);

  cgui_window_on_draw(animation->window, on_draw);

  cgui_window_deactivate(animation->window);
  cgui_window_disable(animation->window);
}

void destroy_animation(animation_t *animation) {
  cgui_window_destroy(animation->window);
  animation->window = NULL;
  cgui_grid_destroy(animation->grid);
  animation->grid = NULL;
}

void activate_animation(animation_t *animation){
  cgui_window_activate(animation->window);
  cgui_window_enable(animation->window);
}

void do_animation(animation_t *animation, enum animation_mode_t mode) {
}
