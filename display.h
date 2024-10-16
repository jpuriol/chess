#include "game_defs.h"

void init_ncurses();
void close_ncurses();

void draw_board(Piece board[BOARD_SIZE][BOARD_SIZE]);
void display_info(int turn);
