#include "game_defs.h"

void init_ncurses();
void close_ncurses();

void clear_display();
void get_move(char *move);
void wait_for_any_key();

void draw_board(Piece board[BOARD_SIZE][BOARD_SIZE]);
void display_info(int turn);
void display_invalid_move();
