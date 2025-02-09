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

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

#include <cassette/cgui.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

static void on_click(cgui_cell *);
static void update_label(void);

/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/

static cstr *score_str = CSTR_PLACEHOLDER;
static cgui_cell *label = CGUI_CELL_PLACEHOLDER;
static cgui_cell *score = CGUI_CELL_PLACEHOLDER;
static cgui_cell *button_1 = CGUI_CELL_PLACEHOLDER;
static cgui_cell *button_2 = CGUI_CELL_PLACEHOLDER;
static cgui_cell *button_3 = CGUI_CELL_PLACEHOLDER;
static cgui_grid *grid = CGUI_GRID_PLACEHOLDER;
static cgui_window *window = CGUI_WINDOW_PLACEHOLDER;

/************************************************************************************************************/
/* QUESTION */
/************************************************************************************************************/

/* Define question_t structure */
typedef struct {
  char question[50];
  int correct_answer; /* index of the correct answer. */
  char answers[3][10];
} question_t;

/* Define player_t structure */
typedef struct {
  int score;
} player_t;

/* Generates a new question */
question_t generate_question() {
  question_t q;
  int num1 = rand() % 10; /* Generates random numbers between 0 and 9 */
  int num2 = rand() % 10;
  int correct_answer = num1 + num2;
  snprintf(q.question, 10, "%d+%d", num1, num2);
  int correct_answer_index = rand() % 3;
  q.correct_answer = correct_answer_index;
  snprintf(q.answers[0], 10, "%d", correct_answer - 1);
  snprintf(q.answers[1], 10, "%d", correct_answer);
  snprintf(q.answers[2], 10, "%d", correct_answer + 1);

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

/* Checks the answer and updates the score */
void check_answer(player_t *player, question_t q, int answer) {
  if (answer == q.correct_answer) {
    player->score++;
  }
}

static question_t question;
static player_t player = {
    .score = 0,
};

/************************************************************************************************************/
/* MAIN */
/************************************************************************************************************/

/**
 * Addition game: click on the right answer to increment the score counter.
 */

int main(int argc, char **argv) {
  /* Instantiation */

  cgui_init(argc, argv);

  window = cgui_window_create();
  grid = cgui_grid_create(3, 3);
  score = cgui_label_create();
  label = cgui_label_create();
  button_1 = cgui_button_create();
  button_2 = cgui_button_create();
  button_3 = cgui_button_create();
  question = generate_question();
  score_str = cstr_create();

  /* Cell setup */

  update_label();
  cgui_label_align(score, CGUI_ALIGN_RIGHT);
  cgui_label_align(label, CGUI_ALIGN_LEFT);

  cgui_button_on_click(button_1, on_click);
  cgui_button_on_click(button_2, on_click);
  cgui_button_on_click(button_3, on_click);

  /* Grid setup */

  cgui_grid_resize_col(grid, 0, 5);
  cgui_grid_resize_col(grid, 1, 5);
  cgui_grid_resize_col(grid, 2, 5);

  cgui_grid_assign_cell(grid, score, 0, 0, 3, 1);
  cgui_grid_assign_cell(grid, label, 0, 1, 3, 1);
  cgui_grid_assign_cell(grid, button_1, 0, 2, 1, 1);
  cgui_grid_assign_cell(grid, button_2, 1, 2, 1, 1);
  cgui_grid_assign_cell(grid, button_3, 2, 2, 1, 1);

  /* Window setup */

  cgui_window_push_grid(window, grid);
  cgui_window_rename(window, "Additions");
  cgui_window_activate(window);

  /* Run */

  cgui_run();

  /* End */

  if (cgui_error()) {
    printf("Gui has failed during operation.\n");
  }

  cgui_window_destroy(window);
  cgui_grid_destroy(grid);
  cgui_cell_destroy(score);
  cgui_cell_destroy(label);
  cgui_cell_destroy(button_1);
  cgui_cell_destroy(button_2);
  cgui_cell_destroy(button_3);

  cgui_reset();

  return 0;
}

/************************************************************************************************************/
/* STATIC */
/************************************************************************************************************/

static void on_click(cgui_cell *c) {
  if (c == button_1) {
    check_answer(&player, question, 0);
  } else if (c == button_2) {
    check_answer(&player, question, 1);
  } else if (c == button_3) {
    check_answer(&player, question, 2);
  }
  update_label();
}

static void update_label(void) {
  cstr_clear(score_str);
  cstr_append(score_str, "score: ");
  cstr_append(score_str, player.score);
  cgui_label_set(score, cstr_chars(score_str));
  question = generate_question();
  cgui_label_set(label, question.question);
  cgui_button_set_label(button_1, question.answers[0]);
  cgui_button_set_label(button_2, question.answers[1]);
  cgui_button_set_label(button_3, question.answers[2]);
}
