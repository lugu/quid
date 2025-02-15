#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Game state
double spaceship_x = WINDOW_WIDTH / 2;
double spaceship_y = WINDOW_HEIGHT / 2;
double spaceship_angle = 0;
double spaceship_speed = 0;

// Spaceship states
typedef enum {
  SPACESHIP_MOVING,      // Normal random movement
  SPACESHIP_MOVING_TO_CENTER, // Moving to the center
  SPACESHIP_EXPLODING,   // During the explosion animation
  SPACESHIP_RESPAWNING    // After the explosion, before new spaceship appears
} SpaceshipState;

SpaceshipState spaceship_state = SPACESHIP_MOVING;

// Laser animation
gboolean laser_active = FALSE;
double laser_y = WINDOW_HEIGHT;
double laser_speed = 20.0;

// Explosion animation
gboolean explosion_active = FALSE;
int explosion_frame = 0; // Current frame of the explosion
const int NUM_EXPLOSION_FRAMES = 10; // Number of frames in the explosion animation

gboolean hit = FALSE;
gint score = 0;

guint respawn_timer_id = 0;

// Function to reset the spaceship to a random location and moving state
gboolean reset_spaceship(gpointer data) {
  spaceship_x = (double)(WINDOW_WIDTH / 2 - 100 + (rand() % 200));
  spaceship_y = (double)(WINDOW_HEIGHT /2 - 100 + (rand() % 200));
  spaceship_angle = 0;
  spaceship_speed = 0;
  spaceship_state = SPACESHIP_MOVING;
  gtk_widget_queue_draw(GTK_WIDGET(data)); // Redraw the window
  return FALSE;
}

// Function to draw the explosion animation
static void draw_explosion(cairo_t *cr) {
  double explosion_radius = 20 + (double)explosion_frame * 15;  // Increase radius over time
  cairo_set_source_rgb(cr, 1.0, 1.0, 0.0); // Yellow for explosion
  cairo_arc(cr, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, explosion_radius, 0, 2 * M_PI);
  cairo_fill(cr);
}

// Function to draw the laser animation
static void draw_laser(cairo_t *cr) {
  cairo_set_source_rgb(cr, 1.0, 0.0, 0.0); // Red
  cairo_set_line_width(cr, 3.0);
  // draw laser 1
  cairo_move_to(cr, 0, laser_y);
  cairo_line_to(cr, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
  cairo_stroke(cr);
  // draw laser 2
  cairo_move_to(cr, WINDOW_WIDTH, laser_y);
  cairo_line_to(cr, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
  cairo_stroke(cr);
}

// Timer function to handle explosion animation frames
gboolean explosion_timer_callback(gpointer data) {
  explosion_frame++;
  if (explosion_frame >= NUM_EXPLOSION_FRAMES) {
    explosion_active = FALSE;
    explosion_frame = 0;
    respawn_timer_id = g_timeout_add(1000, (GSourceFunc)reset_spaceship, data); // 1 second delay
    // Schedule the respawn after a delay
    spaceship_state = SPACESHIP_RESPAWNING;

    // Reset laser
    laser_active = FALSE;
    laser_y = WINDOW_HEIGHT;
    return FALSE;
  }

  gtk_widget_queue_draw(GTK_WIDGET(data)); // Redraw the window
  return TRUE; // Continue the timer
}


// Function to draw the scene
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer data) {
  // Clear the background
  cairo_set_source_rgb(cr, 0, 0, 0); // Black
  cairo_paint(cr);

  int center_x = WINDOW_WIDTH / 2;
  int center_y = WINDOW_HEIGHT / 2;

  // 1. Draw the Arcs (Top and Bottom)
  cairo_set_source_rgb(cr, 0.2, 0.6, 1.0); // Light Blue
  cairo_set_line_width(cr, 2.0);

  double arc_radius = 150; // Adjust for size
  double arc_angle = M_PI / 3; // Angle of the arc

  // Top Arc
  cairo_arc(cr, center_x, center_y - 10, arc_radius, M_PI + arc_angle, 2 * M_PI - arc_angle);
  cairo_stroke(cr);

  // Bottom Arc
  cairo_arc(cr, center_x, center_y + 10, arc_radius, arc_angle, M_PI - arc_angle);
  cairo_stroke(cr);

  // 2. Draw the Horizontal Lines
  cairo_set_source_rgb(cr, 0.2, 0.6, 1.0); // Light Blue
  cairo_set_line_width(cr, 2.0);

  double line_length = 75; // Adjust line length
  double arrow_size = 10;

  // Left Line
  cairo_move_to(cr, center_x - line_length, center_y);
  cairo_line_to(cr, center_x - arrow_size, center_y);
  // Arrow Head
  cairo_line_to(cr, center_x - arrow_size, center_y - arrow_size / 2);
  cairo_move_to(cr, center_x - arrow_size, center_y);
  cairo_line_to(cr, center_x - arrow_size, center_y + arrow_size / 2);
  cairo_stroke(cr);

  // Right Line
  cairo_move_to(cr, center_x + line_length, center_y);
  cairo_line_to(cr, center_x + arrow_size, center_y);
  // Arrow Head
  cairo_line_to(cr, center_x + arrow_size, center_y - arrow_size / 2);
  cairo_move_to(cr, center_x + arrow_size, center_y);
  cairo_line_to(cr, center_x + arrow_size, center_y + arrow_size / 2);
  cairo_stroke(cr);


  // 3. Draw the Spaceship (X-Wing - Simplified) - Only if not exploding or respawning
  if (spaceship_state != SPACESHIP_EXPLODING && spaceship_state != SPACESHIP_RESPAWNING) {
    cairo_set_source_rgb(cr, 1.0, 0.2, 0.2); // Red
    cairo_set_line_width(cr, 2.0);

    // Body
    cairo_rectangle(cr, spaceship_x - 15, spaceship_y - 5, 30, 10);
    cairo_stroke(cr);

    // Wings (Simplified)
    cairo_move_to(cr, spaceship_x - 15, spaceship_y - 5);
    cairo_line_to(cr, spaceship_x - 30, spaceship_y - 15);
    cairo_move_to(cr, spaceship_x + 15, spaceship_y - 5);
    cairo_line_to(cr, spaceship_x + 30, spaceship_y - 15);
    cairo_move_to(cr, spaceship_x - 15, spaceship_y + 5);
    cairo_line_to(cr, spaceship_x - 30, spaceship_y + 15);
    cairo_move_to(cr, spaceship_x + 15, spaceship_y + 5);
    cairo_line_to(cr, spaceship_x + 30, spaceship_y + 15);
    cairo_stroke(cr);
  }

  //draw score
  char score_text[20];
  sprintf(score_text, "Score: %d", score);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 20);
  cairo_move_to(cr, 10, 30);
  cairo_show_text(cr, score_text);

  // Draw laser if active
  if (laser_active) {
    draw_laser(cr);
  }

  // Draw explosion animation
  if (explosion_active) {
    draw_explosion(cr);
  }
  return FALSE;
}

static gboolean update_game(gpointer data) {
  // Initialize random seed (do this only once, typically in main())
  static gboolean initialized = FALSE;
  if (!initialized) {
    srand(time(NULL));
    initialized = TRUE;
  }

  if (spaceship_state == SPACESHIP_MOVING) {
    // 1. Randomly adjust the spaceship's angle
    double angle_change = (double)(rand() % 200 - 100) / 1000.0; // Random change between -0.1 and 0.1
    spaceship_angle += angle_change;

    // 2. Randomly adjust the spaceship's speed
    double speed_change = (double)(rand() % 50 - 25) / 1000.0; // Random change between -0.025 and 0.025
    spaceship_speed += speed_change;

    // 3. Limit the spaceship's speed
    if (spaceship_speed < 5.0) spaceship_speed = 5.0; // Min speed
    if (spaceship_speed > 10.0) spaceship_speed = 10.0; // Max speed

    // 4. Calculate the new position based on angle and speed
    double new_x = spaceship_x + spaceship_speed * cos(spaceship_angle);
    double new_y = spaceship_y + spaceship_speed * sin(spaceship_angle);

    // 5. Keep the spaceship within the bounds of the window (or bounce it off the edges)
    double margin = 250.0; // Distance from the edge to bounce
    if (new_x < margin || new_x > WINDOW_WIDTH - margin) {
      spaceship_angle = M_PI - spaceship_angle; // Reflect horizontally
      new_x = spaceship_x; // Prevent sticking
    }
    if (new_y < margin || new_y > WINDOW_HEIGHT - margin) {
      spaceship_angle = -spaceship_angle; // Reflect vertically
      new_y = spaceship_y; // Prevent sticking
    }

    spaceship_x = new_x;
    spaceship_y = new_y;
  } else if (spaceship_state == SPACESHIP_MOVING_TO_CENTER) {
    // Move the spaceship towards the center
    double dx = WINDOW_WIDTH / 2.0 - spaceship_x;
    double dy = WINDOW_HEIGHT / 2.0 - spaceship_y;
    double distance = sqrt(dx * dx + dy * dy);

    if (distance < 5.0) {
      // Reached the center (approximately) - Start laser animation
      spaceship_x = WINDOW_WIDTH / 2.0;
      spaceship_y = WINDOW_HEIGHT / 2.0;
      if (spaceship_state == SPACESHIP_MOVING_TO_CENTER) {
        laser_active = TRUE;
      }
    } else {
      // Move closer to the center
      double move_speed = 10.0; // Adjust as needed
      spaceship_x += (dx / distance) * move_speed;
      spaceship_y += (dy / distance) * move_speed;
    }
  }

  if (laser_active) {
    laser_y -= laser_speed;
    if (laser_y <= WINDOW_HEIGHT / 2) {
      laser_active = FALSE;
      if (spaceship_state == SPACESHIP_MOVING_TO_CENTER) {
        explosion_active = TRUE;
        spaceship_state = SPACESHIP_EXPLODING;
        g_timeout_add(20, (GSourceFunc)explosion_timer_callback, data);  //Using 20ms as timer interval
      }
    }
  }

  // Redraw the window
  gtk_widget_queue_draw(GTK_WIDGET(data));
  return TRUE;
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
  if (event->keyval == GDK_KEY_space && spaceship_state == SPACESHIP_MOVING) {
    laser_y = WINDOW_HEIGHT;
    spaceship_state = SPACESHIP_MOVING_TO_CENTER;
  }
  if (event->keyval == GDK_KEY_T && spaceship_state == SPACESHIP_MOVING) {
    laser_active = TRUE;
    laser_y = WINDOW_HEIGHT;
  }
  return TRUE;
}

int main(int argc, char *argv[]) {
  // Initialize GTK
  gtk_init(&argc, &argv);
  srand(time(NULL));

  // Create the main window
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Star Wars Target Game");
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Create the drawing area
  GtkWidget *drawing_area = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window), drawing_area);
  gtk_widget_set_events(drawing_area, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK);
  g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), NULL);

  //Connect the key pressed
  gtk_widget_set_can_focus(drawing_area, TRUE);
  gtk_widget_grab_focus(drawing_area);
  g_signal_connect(drawing_area, "key-press-event", G_CALLBACK(on_key_press), drawing_area);

  // Set up the timer
  g_timeout_add(20, update_game, drawing_area);

  // Show the window
  gtk_widget_show_all(window);

  // Start the GTK main loop
  gtk_main();

  return 0;
}
