#include "game_defs.h"

void init_board(Piece board[BOARD_SIZE][BOARD_SIZE]);
int move_piece(Piece board[BOARD_SIZE][BOARD_SIZE], int sx, int sy, int dx, int dy);
void parse_input(char *input, int *sx, int *sy, int *dx, int *dy);