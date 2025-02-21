
/*
 * This animation simulates a Star Wars-like scene with a target and an enemy
 * spaceship.
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "animation.h"

// Window dimensions
const int WINDOW_WIDTH = 524;
const int WINDOW_HEIGHT = 512;

// Number of frames in the explosion animation
const int NUM_EXPLOSION_FRAMES = 10; 

// Spaceship constants
const double SPACESHIP_BODY_WIDTH = 30;
const double SPACESHIP_BODY_HEIGHT = 10;
const double SPACESHIP_WING_LENGTH = 15;

// Laser constants
const double LASER_WIDTH = 3.0;
const double LASER_SPEED = 20.0;

// Explosion constants
const double EXPLOSION_INITIAL_RADIUS = 20;
const double EXPLOSION_RADIUS_INCREMENT = 15;

// Target constants
const double TARGET_ARC_RADIUS = 150;
const double TARGET_ARC_ANGLE = M_PI / 3;
const double TARGET_LINE_LENGTH = 75;
const double TARGET_ARROW_SIZE = 10;

// Movement bounds
const double MOVEMENT_MARGIN = 100.0;

// Spaceship movement constants
const double MIN_SPACESHIP_SPEED = 5.0;
const double MAX_SPACESHIP_SPEED = 10.0;
const double MAX_SPACESHIP_ANGLE = 40.0;

/*
 * Cassette doesn't let us provide private data associated with a window
 * so we have to rely on global to retreive the state of our application
 */
static animation_t *_animation;

void reset_spaceship(spaceship_t *spaceship) {
  spaceship->spaceship_x = (double)(WINDOW_WIDTH / 2 - 100 + (rand() % 200));
  spaceship->spaceship_y = (double)(WINDOW_HEIGHT /2 - 100 + (rand() % 200));
  spaceship->spaceship_draw_angle = 0;
  spaceship->spaceship_move_angle = 0;
  spaceship->spaceship_speed = 0;
  spaceship->spaceship_state = SPACESHIP_MOVING;
}

static void reset_laser(laser_t *laser) {
  laser->laser_active = false;
  laser->laser_y = WINDOW_HEIGHT;
  laser->laser_speed = LASER_SPEED;
}

static void reset_explosion(explosion_t *explosion) {
  explosion->explosion_active = false;
  explosion->explosion_frame = 0;
}

// Function to draw the explosion animation
static void draw_explosion(cairo_t *cr, explosion_t explosion) {
  // Increase radius over time
  double explosion_radius = EXPLOSION_INITIAL_RADIUS + (double)explosion.explosion_frame * EXPLOSION_RADIUS_INCREMENT;  
  // Yellow for explosion
  cairo_set_source_rgb(cr, 1.0, 1.0, 0.0); 
  // Draw a circle
  cairo_arc(cr, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, explosion_radius, 0, 2 * M_PI);
  cairo_fill(cr);
}

// Function to draw the laser animation
static void draw_laser(cairo_t *cr, laser_t laser) {
  cairo_set_source_rgb(cr, 1.0, 0.0, 0.0); // Red
  cairo_set_line_width(cr, LASER_WIDTH);
  // draw laser 1
  cairo_move_to(cr, 0, laser.laser_y);
  cairo_line_to(cr, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
  cairo_stroke(cr);
  // draw laser 2
  cairo_move_to(cr, WINDOW_WIDTH, laser.laser_y);
  cairo_line_to(cr, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
  cairo_stroke(cr);
}

static void draw_spaceship(cairo_t *cr, spaceship_t spaceship) {
  cairo_set_source_rgb(cr, 1.0, 0.2, 0.2); // Red (or your preferred color)
  cairo_set_line_width(cr, 2.0);

  double x = spaceship.spaceship_x;
  double y = spaceship.spaceship_y;
  double angle = spaceship.spaceship_draw_angle; // Use the spaceship's angle for rotation

  cairo_save(cr); // Save current drawing state for rotation

  cairo_translate(cr, x, y); // Move origin to spaceship center
  cairo_rotate(cr, angle);    // Rotate the drawing context

  // Body (Rectangle)
  double body_width = SPACESHIP_BODY_WIDTH;
  double body_height = SPACESHIP_BODY_HEIGHT;
  cairo_rectangle(cr, -body_width / 2, -body_height / 2, body_width, body_height);
  cairo_stroke(cr);

  // Wings (Triangles)
  double wing_length = SPACESHIP_WING_LENGTH;
  cairo_move_to(cr, -body_width / 2, -body_height / 2);
  cairo_line_to(cr, -body_width / 2 - wing_length, -body_height * 1.2); // Adjust these values
  cairo_line_to(cr, -body_width / 2, -body_height);
  cairo_close_path(cr);
  cairo_stroke(cr);

  cairo_move_to(cr, body_width / 2, -body_height / 2);
  cairo_line_to(cr, body_width / 2 + wing_length, -body_height * 1.2); // Adjust these values
  cairo_line_to(cr, body_width / 2, -body_height);
  cairo_close_path(cr);
  cairo_stroke(cr);


  cairo_move_to(cr, -body_width / 2, body_height / 2);
  cairo_line_to(cr, -body_width / 2 - wing_length, body_height * 1.2); // Adjust these values
  cairo_line_to(cr, -body_width / 2, body_height);
  cairo_close_path(cr);
  cairo_stroke(cr);

  cairo_move_to(cr, body_width / 2, body_height / 2);
  cairo_line_to(cr, body_width / 2 + wing_length, body_height * 1.2); // Adjust these values
  cairo_line_to(cr, body_width / 2, body_height);
  cairo_close_path(cr);
  cairo_stroke(cr);

  cairo_restore(cr); // Restore the drawing state (undo rotation/translation)
}

static void draw_target(cairo_t *cr) {
  int center_x = WINDOW_WIDTH / 2;
  int center_y = WINDOW_HEIGHT / 2;

  // 1. Outer Circle
  cairo_set_source_rgb(cr, 0.2, 0.6, 1.0); // Light Blue
  cairo_set_line_width(cr, 2.0);
  cairo_arc(cr, center_x, center_y, TARGET_ARC_RADIUS + 10, 0, 2 * M_PI); // Slightly larger radius
  cairo_stroke(cr);

  // 2. Inner Circle
  cairo_arc(cr, center_x, center_y, TARGET_ARC_RADIUS - 10, 0, 2 * M_PI); // Slightly smaller radius
  cairo_stroke(cr);

  // 3. Horizontal Lines with Ticks
  double line_length = TARGET_LINE_LENGTH;
  double tick_length = 10; // Length of the small ticks
  double tick_offset = 5; // Offset of ticks from the line

  // Left Line
  cairo_move_to(cr, center_x - line_length, center_y);
  cairo_line_to(cr, center_x - TARGET_ARC_RADIUS - 10, center_y); // Extend to outer circle
  // Ticks
  cairo_move_to(cr, center_x - line_length, center_y - tick_length / 2);
  cairo_line_to(cr, center_x - line_length - tick_offset, center_y - tick_length / 2);
  cairo_move_to(cr, center_x - line_length, center_y + tick_length / 2);
  cairo_line_to(cr, center_x - line_length - tick_offset, center_y + tick_length / 2);

  // Right Line
  cairo_move_to(cr, center_x + line_length, center_y);
  cairo_line_to(cr, center_x + TARGET_ARC_RADIUS + 10, center_y); // Extend to outer circle
  // Ticks
  cairo_move_to(cr, center_x + line_length, center_y - tick_length / 2);
  cairo_line_to(cr, center_x + line_length + tick_offset, center_y - tick_length / 2);
  cairo_move_to(cr, center_x + line_length, center_y + tick_length / 2);
  cairo_line_to(cr, center_x + line_length + tick_offset, center_y + tick_length / 2);

  cairo_stroke(cr);


  // 4. Vertical Lines with Ticks
  // Top Line
  cairo_move_to(cr, center_x, center_y - line_length);
  cairo_line_to(cr, center_x, center_y - TARGET_ARC_RADIUS - 10); // Extend to outer circle
  // Ticks
  cairo_move_to(cr, center_x - tick_length / 2, center_y - line_length);
  cairo_line_to(cr, center_x - tick_length / 2, center_y - line_length - tick_offset);
  cairo_move_to(cr, center_x + tick_length / 2, center_y - line_length);
  cairo_line_to(cr, center_x + tick_length / 2, center_y - line_length - tick_offset);

  // Bottom Line
  cairo_move_to(cr, center_x, center_y + line_length);
  cairo_line_to(cr, center_x, center_y + TARGET_ARC_RADIUS + 10); // Extend to outer circle
  // Ticks
  cairo_move_to(cr, center_x - tick_length / 2, center_y + line_length);
  cairo_line_to(cr, center_x - tick_length / 2, center_y + line_length + tick_offset);
  cairo_move_to(cr, center_x + tick_length / 2, center_y + line_length);
  cairo_line_to(cr, center_x + tick_length / 2, center_y + line_length + tick_offset);

  cairo_stroke(cr);
}

// Function to draw the scene
static void draw_animation(animation_t *animation) {
  // 1. Clear the background
  cairo_set_source_rgb(animation->cairo_context, 0, 0, 0); // Black
  cairo_paint(animation->cairo_context);

  // 2. Draw the target
  draw_target(animation->cairo_context);

  // 3. Draw the Spaceship (X-Wing - Simplified) - Only if not exploding or respawning
  if (animation->state.spaceship.spaceship_state == SPACESHIP_MOVING ||
    animation->state.spaceship.spaceship_state == SPACESHIP_MOVING_TO_CENTER) {
    draw_spaceship(animation->cairo_context, animation->state.spaceship);
  }

  // Draw laser if active
  if (animation->state.laser.laser_active) {
    draw_laser(animation->cairo_context, animation->state.laser);
  }

  // Draw explosion animation
  if (animation->state.explosion.explosion_active) {
    draw_explosion(animation->cairo_context, animation->state.explosion);
  }
}

static void update_spaceship_moving(state_t *state) {
  // 1. Randomly adjust the spaceship's direction and angle
  double move_angle_change = (double)(rand() % 200 - 100) / 1000.0; // Random change between -0.1 and 0.1
  state->spaceship.spaceship_move_angle += move_angle_change;

  double draw_angle_change = (double)(rand() % 200 - 100) / 5000.0; // Random change between -0.1 and 0.1
  state->spaceship.spaceship_draw_angle += draw_angle_change;
  state->spaceship.spaceship_draw_angle = fmax(-MAX_SPACESHIP_ANGLE, fmin(state->spaceship.spaceship_draw_angle, MAX_SPACESHIP_ANGLE));

  // 2. Randomly adjust the spaceship's speed
  double speed_change = (double)(rand() % 50 - 25) / 1000.0; // Random change between -0.025 and 0.025
  state->spaceship.spaceship_speed += speed_change;

  // 3. Limit the spaceship's speed
  if (state->spaceship.spaceship_speed < 5.0) state->spaceship.spaceship_speed = 5.0; // Min speed
  if (state->spaceship.spaceship_speed > MAX_SPACESHIP_SPEED) state->spaceship.spaceship_speed = MAX_SPACESHIP_SPEED; // Max speed

  // 4. Calculate the new position based on angle and speed
  double new_x = state->spaceship.spaceship_x + state->spaceship.spaceship_speed * cos(state->spaceship.spaceship_move_angle);
  double new_y = state->spaceship.spaceship_y + state->spaceship.spaceship_speed * sin(state->spaceship.spaceship_move_angle);

  // 5. Keep the spaceship within the bounds of the window (or bounce it off the edges)
  double margin = MOVEMENT_MARGIN;// Distance from the edge to bounce 
  if (new_x < margin || new_x > WINDOW_WIDTH - margin) {
    state->spaceship.spaceship_move_angle = M_PI - state->spaceship.spaceship_move_angle; // Reflect horizontally
    new_x = state->spaceship.spaceship_x; // Prevent sticking
  }
  if (new_y < margin || new_y > WINDOW_HEIGHT - margin) {
    state->spaceship.spaceship_move_angle = -state->spaceship.spaceship_move_angle; // Reflect vertically
    new_y = state->spaceship.spaceship_y; // Prevent sticking
  }

  state->spaceship.spaceship_x = new_x;
  state->spaceship.spaceship_y = new_y;
}

static void update_spaceship_moving_to_center(state_t *state) {
  // Move the spaceship towards the center
  double dx = WINDOW_WIDTH / 2.0 - state->spaceship.spaceship_x;
  double dy = WINDOW_HEIGHT / 2.0 - state->spaceship.spaceship_y;
  double distance = sqrt(dx * dx + dy * dy);

  if (distance < 5.0) {
    // Reached the center (approximately) - Start laser animation
    state->spaceship.spaceship_x = WINDOW_WIDTH / 2.0;
    state->spaceship.spaceship_y = WINDOW_HEIGHT / 2.0;
    if (state->spaceship.spaceship_state == SPACESHIP_MOVING_TO_CENTER) {
      state->laser.laser_active = true;
    }
  } else {
    // Move closer to the center
    double move_speed = MAX_SPACESHIP_SPEED; // Adjust as needed
    state->spaceship.spaceship_x += (dx / distance) * move_speed;
    state->spaceship.spaceship_y += (dy / distance) * move_speed;
  }
}

static void update_explosion(state_t *state) {
  if (state->explosion.explosion_active) {
    state->explosion.explosion_frame++;
    if (state->explosion.explosion_frame >= NUM_EXPLOSION_FRAMES) {
      // Reset everything
      reset_laser(&state->laser);
      reset_explosion(&state->explosion);
      reset_spaceship(&state->spaceship);
    }
  }
}

static void update_laser(state_t *state) {
  if (state->laser.laser_active) {
    state->laser.laser_y -= state->laser.laser_speed;
    if (state->laser.laser_y <= WINDOW_HEIGHT / 2) {
      state->laser.laser_active = false;
      if (state->spaceship.spaceship_state == SPACESHIP_MOVING_TO_CENTER) {
        state->explosion.explosion_active = true;
        state->spaceship.spaceship_state = SPACESHIP_EXPLODING;
      }
    }
  }
}

static void update_animation(animation_t *animation, unsigned long delay) {
  switch (animation->state.spaceship.spaceship_state) {
    case SPACESHIP_MOVING:
      update_spaceship_moving(&animation->state);
      break;
    case SPACESHIP_MOVING_TO_CENTER:
      update_spaceship_moving_to_center(&animation->state);
      break;
    case SPACESHIP_EXPLODING:
      update_explosion(&animation->state);
      break;
    case SPACESHIP_RESPAWNING:
      reset_spaceship(&animation->state.spaceship);
      break;
  }
  update_laser(&animation->state);
}

static void on_draw(cgui_window *window, unsigned long delay) {
  update_animation(_animation, delay);
  draw_animation(_animation);
  cgui_window_redraw_delayed(window, 20 /* milliseconds */);
}

void init_animation(animation_t *animation, cgui_window *parent_window) {
  _animation = animation;
  animation->parent_window = parent_window;
  animation->window = cgui_window_create();
  animation->grid = cgui_grid_create(1, 1);
  animation->cairo_context = cgui_window_cairo_drawable(animation->window);
  // Height and width will be known when the window is active.
  animation->surface_height = 0;
  animation->surface_width = 0;

  reset_spaceship(&animation->state.spaceship);
  reset_laser(&animation->state.laser);
  reset_explosion(&animation->state.explosion);

  cgui_grid_resize_col(animation->grid, 0, 50);
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
  int surface_height;
  int surface_width;

  cgui_window_activate(animation->window);
  cgui_window_enable(animation->window);

  surface_height = cgui_window_height(animation->window);
  surface_width = cgui_window_width(animation->window);
  if (surface_height != WINDOW_HEIGHT) {
    fprintf(stderr, "unexpected window height: %d\n", surface_height);
  }
  if (surface_width != WINDOW_WIDTH) {
    fprintf(stderr, "unexpected window width: %d\n", surface_width);
  }
}

void do_animation(animation_t *animation, enum animation_mode_t mode) {
  switch(mode) {
    case ANIMATION_SUCCESS:
      if (animation->state.spaceship.spaceship_state == SPACESHIP_MOVING) {
        animation->state.laser.laser_y = WINDOW_HEIGHT;
        animation->state.spaceship.spaceship_state = SPACESHIP_MOVING_TO_CENTER;
      }
      break;
    case ANIMATION_FAILURE:
      if (animation->state.spaceship.spaceship_state == SPACESHIP_MOVING) {
        animation->state.laser.laser_active = true;
        animation->state.laser.laser_y = WINDOW_HEIGHT;
      }
      break;
  }
}
