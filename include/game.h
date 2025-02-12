#include <cassette/cgui.h>
//
// Use a larger buffer size for strings
#define MAX_ANSWER_LENGTH 10
#define MAX_QUESTION_LENGTH 50

typedef struct {
  int correct_answer;
  char question[MAX_QUESTION_LENGTH];
  char answers[3][MAX_ANSWER_LENGTH];
} question_t;

enum game_mode_t { ADDITION, MULTIPLICATION };

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
  cgui_window *parent_window;
  question_t question;
  enum game_mode_t mode;
} game_t;

void init_game(game_t *game, cgui_window *parent_window);
void destroy_game(game_t *game);
void reset_game(game_t *game, enum game_mode_t mode);
