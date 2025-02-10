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

#include <cassette/cgui.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char question[50];
  int correct_answer;
  char answers[3][10];
} question_t;

typedef struct {
  int score;
  cstr *score_str;
  cgui_cell *label;
  cgui_cell *score_label;
  cgui_cell *gauge;
  cgui_cell *button_1;
  cgui_cell *button_2;
  cgui_cell *button_3;
  cgui_grid *grid;
  cgui_window *window;
  question_t question;
} game_t;

static question_t generate_question() {
  question_t q;
  int num1 = rand() % 20; /* Generates random numbers between 0 and 9 */
  int num2 = rand() % 20;
  int correct_answer = num1 + num2;
  int incorrect_answer_1, incorrect_answer_2;
  snprintf(q.question, 10, "%d+%d", num1, num2);
  int correct_answer_index = rand() % 3;
  q.correct_answer = correct_answer_index;
  if (rand() % 2) {
    incorrect_answer_1 = correct_answer - 1;
  } else {
    incorrect_answer_1 = correct_answer + 1;
  }
  if (rand() % 2) {
    incorrect_answer_2 = correct_answer - 2;
  } else {
    incorrect_answer_2 = correct_answer + 2;
  }
  snprintf(q.answers[0], 10, "%d", incorrect_answer_1);
  snprintf(q.answers[1], 10, "%d", correct_answer);
  snprintf(q.answers[2], 10, "%d", incorrect_answer_2);

  /* Shuffle the answers array */
  if (correct_answer_index != 1) {
    int temp = correct_answer_index;
    char temp_str[10];
    strncpy(temp_str, q.answers[1], 10);
    strncpy(q.answers[1], q.answers[temp], 10);
    strncpy(q.answers[temp], temp_str, 10);
  }

  return q;
}

static void check_answer(game_t *game, int answer) {
  if (answer == game->question.correct_answer) {
    game->score++;
  }
}

static void update_label(game_t *game) {
  cstr_clear(game->score_str);
  cstr_append(game->score_str, "score: ");
  cstr_append(game->score_str, game->score);
  cgui_label_set(game->score_label, cstr_chars(game->score_str));
  cgui_gauge_set_value(game->gauge, 2.0 * game->score);
  game->question = generate_question();
  cgui_label_set(game->label, game->question.question);
  cgui_button_set_label(game->button_1, game->question.answers[0]);
  cgui_button_set_label(game->button_2, game->question.answers[1]);
  cgui_button_set_label(game->button_3, game->question.answers[2]);
}

static void on_click(cgui_cell *c, void *g) {
  game_t *game = (game_t *)g;
  if (c == game->button_1) {
    check_answer(game, 0);
  } else if (c == game->button_2) {
    check_answer(game, 1);
  } else if (c == game->button_3) {
    check_answer(game, 2);
  }
  update_label(game);
}

int main(int argc, char **argv) {
  game_t game;

  srand(time(NULL));
  cgui_init(argc, argv);

  game.score = 0;
  game.score_str = cstr_create();
  game.label = cgui_label_create();
  game.score_label = cgui_label_create();
  game.gauge = cgui_gauge_create();
  game.button_1 = cgui_button_create();
  game.button_2 = cgui_button_create();
  game.button_3 = cgui_button_create();
  game.grid = cgui_grid_create(4, 3);
  game.window = cgui_window_create();
  game.question = generate_question();

  cgui_label_align(game.score_label, CGUI_ALIGN_RIGHT);
  cgui_label_align(game.label, CGUI_ALIGN_LEFT);

  cgui_button_on_click(game.button_1, on_click, (void *)&game);
  cgui_button_on_click(game.button_2, on_click, (void *)&game);
  cgui_button_on_click(game.button_3, on_click, (void *)&game);

  cgui_gauge_clamp_value(game.gauge, 0.0, 100.0);
  cgui_gauge_hide_label(game.gauge);
  cgui_gauge_rotate(game.gauge, CGUI_ROTATION_RIGHT);

  update_label(&game);

  cgui_grid_resize_col(game.grid, 0, 5);
  cgui_grid_resize_col(game.grid, 1, 5);
  cgui_grid_resize_col(game.grid, 2, 5);
  cgui_grid_resize_col(game.grid, 3, 5);

  cgui_grid_assign_cell(game.grid, game.score_label, 0, 0, 3, 1);
  cgui_grid_assign_cell(game.grid, game.label, 0, 1, 3, 1);
  cgui_grid_assign_cell(game.grid, game.gauge, 3, 0, 1, 3);
  cgui_grid_assign_cell(game.grid, game.button_1, 0, 2, 1, 1);
  cgui_grid_assign_cell(game.grid, game.button_2, 1, 2, 1, 1);
  cgui_grid_assign_cell(game.grid, game.button_3, 2, 2, 1, 1);

  cgui_window_push_grid(game.window, game.grid);
  cgui_window_rename(game.window, "Additions");
  cgui_window_activate(game.window);

  cgui_run();

  if (cgui_error()) {
    printf("Gui has failed during operation.\n");
  }

  cgui_window_destroy(game.window);
  cgui_grid_destroy(game.grid);
  cgui_cell_destroy(game.score_label);
  cgui_cell_destroy(game.label);
  cgui_cell_destroy(game.gauge);
  cgui_cell_destroy(game.button_1);
  cgui_cell_destroy(game.button_2);
  cgui_cell_destroy(game.button_3);

  cgui_reset();

  return 0;
}
