
/*
 * This game presents the user with math questions (addition or multiplication).
 *
 * Game Modes:
 *   - ADDITION: Generates two random numbers between 1 and 20, asks for their sum.
 *   - MULTIPLICATION: Generates two random numbers between 0 and 9, asks for their product.
 *
 * Scoring:
 *   - Correct answer: Score increases by 1.
 *   - Wrong answer: Life decreases by 1.
 *
 * Life:
 *   - The player starts with 10 lives.
 *   - Game over when lives reach 0.
 *
 * Wrong Answers:
 *   - Two incorrect answers are generated.
 *   - One is the correct answer - 1 or + 1
 *   - The other is the correct answer - 2 or + 2
 *   - The correct answer and incorrect answers are randomly shuffled
 */

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "game.h"

static question_t generate_question(enum game_mode_t mode) {
  question_t q;
  int num1, num2;
  int correct_answer, incorrect_answer_1, incorrect_answer_2;
  switch (mode) {
  case ADDITION:
    num1 = 1 + rand() % 20; /* Generates random numbers between 1 and 20 */
    num2 = 1 + rand() % 20;
    correct_answer = num1 + num2;
    snprintf(q.question, MAX_ANSWER_LENGTH, "%d+%d", num1, num2);
    break;
  case MULTIPLICATION:
    num1 = rand() % 10; /* Generates random numbers between 0 and 9 */
    num2 = rand() % 10;
    correct_answer = num1 * num2;
    snprintf(q.question, MAX_ANSWER_LENGTH, "%d*%d", num1, num2);
    break;
  }
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
  snprintf(q.answers[0], MAX_ANSWER_LENGTH, "%d", incorrect_answer_1);
  snprintf(q.answers[1], MAX_ANSWER_LENGTH, "%d", correct_answer);
  snprintf(q.answers[2], MAX_ANSWER_LENGTH, "%d", incorrect_answer_2);

  /* Shuffle the answers array */
  if (correct_answer_index != 1) {
    int temp = correct_answer_index;
    char temp_str[MAX_ANSWER_LENGTH];
    strncpy(temp_str, q.answers[1], MAX_ANSWER_LENGTH);
    strncpy(q.answers[1], q.answers[temp], MAX_ANSWER_LENGTH);
    strncpy(q.answers[temp], temp_str, MAX_ANSWER_LENGTH);
  }

  return q;
}

static void check_answer(game_t *game, int answer) {
  if (answer == game->question.correct_answer) {
    game->score++;
    do_animation(&game->animation, ANIMATION_SUCCESS);
  } else {
    game->life--;
    do_animation(&game->animation, ANIMATION_FAILURE);
  }
}

static void update_label(game_t *game) {
  cstr_clear(game->score_str);
  cstr_append(game->score_str, "score: ");
  cstr_append(game->score_str, game->score);
  cgui_label_set(game->score_label, cstr_chars(game->score_str));
  cgui_gauge_set_value(game->gauge, 10 * game->life);
  game->question = generate_question(game->mode);
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

static void on_close(cgui_window *w) { cgui_window_deactivate(w); }

void init_game(game_t *game, cgui_window *parent_window) {
  game->score_str = cstr_create();
  game->label = cgui_label_create();
  game->score_label = cgui_label_create();
  game->gauge = cgui_gauge_create();
  game->button_1 = cgui_button_create();
  game->button_2 = cgui_button_create();
  game->button_3 = cgui_button_create();
  game->grid = cgui_grid_create(4, 3);
  game->window = cgui_window_create();
  game->parent_window = parent_window;
  game->question = generate_question(game->mode);

  reset_game(game, ADDITION);

  init_animation(&game->animation, game->window);

  cgui_label_align(game->score_label, CGUI_ALIGN_RIGHT);
  cgui_label_align(game->label, CGUI_ALIGN_LEFT);

  cgui_button_on_click(game->button_1, on_click, (void *)game);
  cgui_button_on_click(game->button_2, on_click, (void *)game);
  cgui_button_on_click(game->button_3, on_click, (void *)game);

  cgui_gauge_clamp_value(game->gauge, 0.0, 100.0);
  cgui_gauge_hide_label(game->gauge);
  cgui_gauge_rotate(game->gauge, CGUI_ROTATION_RIGHT);

  cgui_grid_resize_col(game->grid, 0, 5);
  cgui_grid_resize_col(game->grid, 1, 5);
  cgui_grid_resize_col(game->grid, 2, 5);
  cgui_grid_resize_col(game->grid, 3, 5);

  cgui_grid_assign_cell(game->grid, game->score_label, 0, 0, 3, 1);
  cgui_grid_assign_cell(game->grid, game->label, 0, 1, 3, 1);
  cgui_grid_assign_cell(game->grid, game->gauge, 3, 0, 1, 3);
  cgui_grid_assign_cell(game->grid, game->button_1, 0, 2, 1, 1);
  cgui_grid_assign_cell(game->grid, game->button_2, 1, 2, 1, 1);
  cgui_grid_assign_cell(game->grid, game->button_3, 2, 2, 1, 1);

  cgui_window_push_grid(game->window, game->grid);
  cgui_window_rename(game->window, "Additions");
  cgui_window_on_close(game->window, on_close);

  cgui_window_deactivate(game->window);
  cgui_window_disable(game->window);
}

void activate_game(game_t *game) {
  cgui_window_activate(game->window);
  cgui_window_enable(game->window);
  activate_animation(&game->animation);
}

void destroy_game(game_t *game) {
  destroy_animation(&game->animation);

  cgui_window_destroy(game->window);
  game->window = NULL;
  cgui_grid_destroy(game->grid);
  game->grid = NULL;
  cgui_cell_destroy(game->score_label);
  game->score_label = NULL;
  cgui_cell_destroy(game->label);
  game->label = NULL;
  cgui_cell_destroy(game->gauge);
  game->gauge = NULL;
  cgui_cell_destroy(game->button_1);
  game->button_1 = NULL;
  cgui_cell_destroy(game->button_2);
  game->button_2 = NULL;
  cgui_cell_destroy(game->button_3);
  game->button_3 = NULL;
}

void reset_game(game_t *game, enum game_mode_t mode) {
  game->mode = mode;
  game->score = 0.0;
  game->life = 10.0;
  game->question = generate_question(game->mode);
  update_label(game);
}
