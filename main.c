#include <ncurses.h>
#include <stdlib.h>

#include <string.h>
#include "display.h"
#include "logic.h"

Piece board[BOARD_SIZE][BOARD_SIZE];
int turn = WHITE; // Start with WHITE player

int main()
{
    init_ncurses();

    init_board(board);

    char input[6];
    int sx, sy, dx, dy;

    while (1)
    {
        clear();
        draw_board(board);  // Draw the board with colored pieces and labels
        display_info(turn); // Show current player and instructions

        getstr(input); // Get the move input from the user

        // Quit the game if the user inputs "q"
        if (strcmp(input, "q") == 0)
        {
            break;
        }

        // Parse the input into source (sx, sy) and destination (dx, dy)
        parse_input(input, &sx, &sy, &dx, &dy);

        // Attempt the move
        if (move_piece(board, sx, sy, dx, dy))
        {
            turn = (turn == WHITE) ? BLACK : WHITE; // Switch player after a valid move
        }
        else
        {
            mvprintw(38, 5, "Invalid move! Press any key to try again...");
            getch();
        }
    }

    close_ncurses();
    return 0;
}
