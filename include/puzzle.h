#include <xcb/xcb.h>
#include <cassette/cgui-window.h>

#define TILE_WIDTH 32
#define TILE_HEIGHT 32

typedef struct {
  bool board[TILE_WIDTH][TILE_HEIGHT];
  cgui_window *window;
  xcb_window_t x11_window;
  xcb_connection_t *connection;
  xcb_gcontext_t graphics_context;
  xcb_pixmap_t image; /* ignore the image for now */
} puzzle_t;

void init_puzzle(puzzle_t *puzzle);
bool is_finished(puzzle_t *puzzle);
void remove_one_piece(puzzle_t *puzzle);
void redraw_puzzle(puzzle_t *puzzle);
void destroy_puzzle(puzzle_t *puzzle);
