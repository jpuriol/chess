    #include "logic.h"

#include <string.h>

void init_board(Piece board[BOARD_SIZE][BOARD_SIZE])
{
    // Initialize empty board
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            board[y][x].type = EMPTY;
            board[y][x].player = NONE;
        }
    }

    // Place pawns
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        board[1][x].type = PAWN;
        board[1][x].player = WHITE;
        board[6][x].type = PAWN;
        board[6][x].player = BLACK;
    }

    // Place rooks
    board[0][0].type = board[0][7].type = ROOK;
    board[0][0].player = board[0][7].player = WHITE;
    board[7][0].type = board[7][7].type = ROOK;
    board[7][0].player = board[7][7].player = BLACK;

    // Place knights
    board[0][1].type = board[0][6].type = KNIGHT;
    board[0][1].player = board[0][6].player = WHITE;
    board[7][1].type = board[7][6].type = KNIGHT;
    board[7][1].player = board[7][6].player = BLACK;

    // Place bishops
    board[0][2].type = board[0][5].type = BISHOP;
    board[0][2].player = board[0][5].player = WHITE;
    board[7][2].type = board[7][5].type = BISHOP;
    board[7][2].player = board[7][5].player = BLACK;

    // Place queens
    board[0][3].type = QUEEN;
    board[0][3].player = WHITE;
    board[7][3].type = QUEEN;
    board[7][3].player = BLACK;

    // Place kings
    board[0][4].type = KING;
    board[0][4].player = WHITE;
    board[7][4].type = KING;
    board[7][4].player = BLACK;
}

// Validate the move (simple version)
int is_valid_move(Piece board[BOARD_SIZE][BOARD_SIZE], int sx, int sy, int dx, int dy)
{
    // Add simple move validation for now (e.g., no out-of-bound or same-player destination)
    if (dx < 0 || dx >= BOARD_SIZE || dy < 0 || dy >= BOARD_SIZE)
        return 0;
    if (board[sy][sx].player == board[dy][dx].player)
        return 0; // Can't move to a spot with the same player

    // Add specific movement rules for each piece here (e.g., pawns move forward, knights move in L shape)

    return 1; // Accepting all moves for now
}

int move_piece(Piece board[BOARD_SIZE][BOARD_SIZE], int sx, int sy, int dx, int dy)
{
    if (is_valid_move(board, sx, sy, dx, dy))
    {
        board[dy][dx] = board[sy][sx];
        board[sy][sx].type = EMPTY;
        board[sy][sx].player = NONE;
        return 1;
    }
    return 0;
}

// Parse the user input (e.g., "e2e4") and convert it into board coordinates
void parse_input(char *input, int *sx, int *sy, int *dx, int *dy)
{
    if (strlen(input) != 4)
    {
        *sx = *sy = *dx = *dy = -1; // Invalid input
        return;
    }

    *sx = input[0] - 'a'; // 'a' -> 0, 'b' -> 1, ..., 'h' -> 7
    *sy = '8' - input[1]; // '8' -> 0, '7' -> 1, ..., '1' -> 7
    *dx = input[2] - 'a';
    *dy = '8' - input[3];
}