#include <xcb/xcb_image.h>

#include <cassette/cgui.h>

#include <stddef.h>
#include <unistd.h>

#include "puzzle.h"

/**
 * A "puzzle" is made of an image (loaded from a file) displayed into an X11
 * window (xcb_window_t). On top of the image is a set of square tiles that
 * hide the image.
 *
 * Each tile can be removed one by one to uncover the image. It is by games to
 * represent the progression of the player: each time a player give a right 
 * answer, a tile is removed.
 */

static puzzle_t *g_puzzle_ptr;

static char tile_data[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0xff, 0xff, 0x04,
   0x40, 0x10, 0x10, 0x02, 0x80, 0x28, 0x28, 0x01, 0xff, 0x49, 0x24, 0xff,
   0x06, 0x44, 0x44, 0x60, 0x18, 0x84, 0x42, 0x18, 0x60, 0x82, 0x83, 0x06,
   0xff, 0x02, 0x80, 0xff, 0xff, 0x22, 0x88, 0xff, 0x0f, 0x22, 0x88, 0x78,
   0xff, 0x22, 0x88, 0xff, 0xff, 0x02, 0x80, 0xff, 0xff, 0x3a, 0xb9, 0xff,
   0xff, 0x04, 0x40, 0xff, 0xff, 0x08, 0x20, 0xff, 0xff, 0x70, 0x1c, 0x02,
   0xff, 0x40, 0x04, 0x05, 0xff, 0x20, 0x88, 0x04, 0xff, 0x10, 0x50, 0x02,
   0xff, 0x08, 0x20, 0x01, 0xff, 0x0b, 0xa0, 0x01, 0x80, 0x0c, 0x61, 0x02,
   0x40, 0x18, 0x31, 0x04, 0x40, 0x10, 0x11, 0x04, 0xc0, 0x11, 0x11, 0x07,
   0x60, 0x90, 0x13, 0x0c, 0xe0, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static void on_draw(cgui_window *window, unsigned long delay) {
  printf("on draw\n");
  redraw_puzzle(g_puzzle_ptr);
}

void init_puzzle(puzzle_t *puzzle) {
	cgui_grid *grid;
  puzzle->window = cgui_window_create();
  puzzle->x11_window = cgui_window_x11_id(puzzle->window);
  puzzle->connection = cgui_x11_connection();
  puzzle->graphics_context = xcb_generate_id(puzzle->connection);
  for (int i = 0; i < TILE_WIDTH; i++) {
    for (int j = 0; j < TILE_HEIGHT; j++) {
      puzzle->board[i][j] = true;
    }
  }
  // TODO: initialize the image
	grid = cgui_grid_create(1, 1);
	cgui_window_push_grid(puzzle->window, grid);
  cgui_window_activate(puzzle->window);
  g_puzzle_ptr = puzzle;
  cgui_window_on_draw(puzzle->window, on_draw);
}

bool is_finished(puzzle_t *puzzle) {
  for (int i = 0; i < TILE_WIDTH; i++) {
    for (int j = 0; j < TILE_HEIGHT; j++) {
      if (puzzle->board[i][j]) {
        return false;
      }
    }
  }
  return true;
}

void remove_one_piece(puzzle_t *puzzle) {
  // TODO: remove the piece randomely
  for (int i = 0; i < TILE_WIDTH; i++) {
    for (int j = 0; j < TILE_HEIGHT; j++) {
      if (puzzle->board[i][j]) {
        puzzle->board[i][j] = false;
        return;
      }
    }
  }
}

void redraw_puzzle(puzzle_t *puzzle) {
  // TODO: draw the image.
  for (int i = 0; i < TILE_WIDTH; i++) {
    for (int j = 0; j < TILE_HEIGHT; j++) {
      if (puzzle->board[i][j]) {
        xcb_put_image(puzzle->connection, 
                      XCB_IMAGE_FORMAT_XY_BITMAP, 
                      puzzle->x11_window, 
                      puzzle->graphics_context, 
                      TILE_WIDTH, TILE_HEIGHT, 
                      i * TILE_WIDTH, 
                      j * TILE_HEIGHT, 
                      0, 0, 
                      sizeof(tile_data),
                      (const uint8_t *)tile_data);
      }
    }
  }
}

void destroy_puzzle(puzzle_t *puzzle) {
  // TODO: free the tile
}
