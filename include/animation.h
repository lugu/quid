#include <cassette/cgui-window.h>
#include <cassette/cgui.h>
#include <cairo/cairo.h>

#include "sound.h"

// Spaceship states
typedef enum {
  SPACESHIP_MOVING,           // Normal random movement
  SPACESHIP_MOVING_TO_CENTER, // Moving to the center
  SPACESHIP_EXPLODING,        // During the explosion animation
  SPACESHIP_GONE              // Do not display the spaceship
} spaceship_state_e;

typedef struct {
  double spaceship_x;
  double spaceship_y;
  double spaceship_speed;
  double spaceship_draw_angle;
  double spaceship_move_angle;
  spaceship_state_e spaceship_state;
} spaceship_t;

typedef struct {
  bool laser_active;
  double laser_y;
  double laser_speed;
} laser_t;

typedef struct {
  bool explosion_active;
  int explosion_frame;
} explosion_t;

typedef struct {
  spaceship_t spaceship;
  explosion_t explosion;
  laser_t laser;
} state_t;

typedef struct {
  cgui_window *window;
  cgui_window *parent_window;
  cgui_grid *grid;
  cairo_t *cairo_context;
  int surface_width;
  int surface_height;
  state_t state;
  sound_effects_t sound;
} animation_t;

enum animation_mode_t {
  ANIMATION_SUCCESS,
  ANIMATION_FAILURE,
  ANIMATION_END,
};

void init_animation(animation_t *animation, cgui_window *parent_window);
void destroy_animation(animation_t *animation);
void activate_animation(animation_t *animation);
void do_animation(animation_t *animation, enum animation_mode_t mode);
void reset_animation(animation_t *animation);
