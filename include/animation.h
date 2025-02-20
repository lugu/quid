#include <cassette/cgui-window.h>
#include <cassette/cgui.h>
#include <cairo/cairo.h>

// Spaceship states
typedef enum {
  SPACESHIP_MOVING,      // Normal random movement
  SPACESHIP_MOVING_TO_CENTER, // Moving to the center
  SPACESHIP_EXPLODING,   // During the explosion animation
  SPACESHIP_RESPAWNING    // After the explosion, before new spaceship appears
} spaceship_state_e;

typedef struct {
  double spaceship_x;
  double spaceship_y;
  double spaceship_angle;
  double spaceship_speed;
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
} animation_t;

enum animation_mode_t { ANIMATION_SUCCESS, ANIMATION_FAILURE };

void init_animation(animation_t *animation, cgui_window *parent_window);
void destroy_animation(animation_t *animation);
void activate_animation(animation_t *animation);
void do_animation(animation_t *animation, enum animation_mode_t mode);
