/**
 * This program is free software; you can redistribute it and/or modify it
 * either under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3.0 of the License or (at
 * your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassette/cgui-window.h>
#include <cassette/cgui.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include "game.h"
#include "puzzle.h"

typedef struct {
  cgui_window *window;
  cgui_grid *grid;
  cgui_cell *add;
  cgui_cell *mul;
  game_t game;
  puzzle_t puzzle;
} app_t;

static void on_click_add(cgui_cell *c, void *a) {
  app_t *app = (app_t *)(a);
  reset_game(&app->game, ADDITION);
  cgui_window_activate(app->game.window);
  cgui_window_enable(app->game.window);
}

static void on_click_mul(cgui_cell *c, void *a) {
  app_t *app = (app_t *)(a);
  reset_game(&app->game, MULTIPLICATION);
  cgui_window_activate(app->game.window);
  cgui_window_enable(app->game.window);
}

int main(int argc, char **argv) {
  app_t app;

  srand(time(NULL));
  cgui_init(argc, argv);

  /* instanciate elements */
  app.window = cgui_window_create();
  app.grid = cgui_grid_create(1, 2);
  app.add = cgui_button_create();
  app.mul = cgui_button_create();

  /* cell setup */
  cgui_button_set_label(app.add, "Additions");
  cgui_button_set_label(app.mul, "Multiplications");

  cgui_button_on_click(app.add, on_click_add, (void *)(&app));
  cgui_button_on_click(app.mul, on_click_mul, (void *)(&app));

  /* grid setup */
  cgui_grid_resize_col(app.grid, 0, 20);
  cgui_grid_assign_cell(app.grid, app.add, 0, 0, 1, 1);
  cgui_grid_assign_cell(app.grid, app.mul, 0, 1, 1, 1);

  cgui_window_push_grid(app.window, app.grid);
  cgui_window_rename(app.window, "Number games");
  cgui_window_activate(app.window);

  init_game(&app.game, app.window);
  init_puzzle(&app.puzzle);
  // FIXME: is it needed?
  redraw_puzzle(&app.puzzle);
  cgui_window_deactivate(app.game.window);
  cgui_window_disable(app.game.window);

  cgui_run();

  if (cgui_error()) {
    printf("Gui has failed during operation.\n");
  }

  destroy_game(&app.game);
  destroy_puzzle(&app.puzzle);
  cgui_window_destroy(app.window);
  cgui_grid_destroy(app.grid);
  cgui_cell_destroy(app.add);
  cgui_cell_destroy(app.mul);

  return 0;
}
