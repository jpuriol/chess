#include "display.h"

#include <ncurses.h>
#include <locale.h>
#include <wchar.h>

#define CUSTOM_GREY 8

void init_ncurses()
{
    // Set the locale for Unicode support
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    // Initialize colors
    start_color();
    // Define color pairs: foreground (text color), background (background color)
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // White pieces: white text on black
    init_pair(2, COLOR_BLACK, COLOR_WHITE); // Black pieces: black text on white

    // Define colors for the chessboard squares
    init_color(CUSTOM_GREY, 700, 700, 700);
    init_color(COLOR_YELLOW, 500, 300, 150); // Light brown (custom color)
    init_pair(3, CUSTOM_GREY, COLOR_YELLOW); // Light square (brownish)
    init_color(COLOR_RED, 400, 200, 100);    // Dark brown (custom color)
    init_pair(4, CUSTOM_GREY, COLOR_RED);    // Dark square (brownish)
}

void close_ncurses()
{
    endwin();
}

wchar_t *unicode_piece_char(Piece piece)
{
    wchar_t *piece_char = L" ";

    // Use Unicode characters for white and black pieces
    if (piece.player == WHITE)
    {
        switch (piece.type)
        {
        case PAWN:
            piece_char = L"♙"; // Unicode for white pawn
            break;
        case ROOK:
            piece_char = L"♖"; // Unicode for white rook
            break;
        case KNIGHT:
            piece_char = L"♘"; // Unicode for white knight
            break;
        case BISHOP:
            piece_char = L"♗"; // Unicode for white bishop
            break;
        case QUEEN:
            piece_char = L"♕"; // Unicode for white queen
            break;
        case KING:
            piece_char = L"♔"; // Unicode for white king
            break;
        }
    }
    else if (piece.player == BLACK)
    {
        switch (piece.type)
        {
        case PAWN:
            piece_char = L"♟"; // Unicode for black pawn
            break;
        case ROOK:
            piece_char = L"♜"; // Unicode for black rook
            break;
        case KNIGHT:
            piece_char = L"♞"; // Unicode for black knight
            break;
        case BISHOP:
            piece_char = L"♝"; // Unicode for black bishop
            break;
        case QUEEN:
            piece_char = L"♛"; // Unicode for black queen
            break;
        case KING:
            piece_char = L"♚"; // Unicode for black king
            break;
        }
    }

    return piece_char;
}

void draw_board(Piece board[BOARD_SIZE][BOARD_SIZE])
{
    // Offsets and measurements for the board
    int offset_x = 8;               // Horizontal offset for the board
    int offset_y = 2;               // Vertical offset for the board
    int square_width = 6;           // Width of each square (including borders)
    int square_height = 4;          // Height of each square (including borders)
    int label_offset_x = 3;         // Offset to center labels over squares
    int label_offset_y_top = 1;     // Vertical position for the top labels
    int label_offset_y_bottom = 35; // Vertical position for the bottom labels

    // Draw the column labels (a-h) centered above and below the board
    for (int x = 0; x < BOARD_SIZE; x++)
    {
        // Print the top and bottom column labels with the same calculation for x-offset
        mvprintw(label_offset_y_top, offset_x + (x * square_width) + label_offset_x, "%c", 'a' + x);    // Top labels
        mvprintw(label_offset_y_bottom, offset_x + (x * square_width) + label_offset_x, "%c", 'a' + x); // Bottom labels
    }

    // Draw the board with pieces and row/column labels
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        int y_offset = (y * square_height) + offset_y;

        // Draw the row label (1-8) on the left and right side of the board
        mvprintw(y_offset + 2, offset_x - 2, "%d", BOARD_SIZE - y);                               // Left row labels
        mvprintw(y_offset + 2, offset_x + (BOARD_SIZE * square_width) + 2, "%d", BOARD_SIZE - y); // Right row labels

        for (int x = 0; x < BOARD_SIZE; x++)
        {
            int x_offset = offset_x + (x * square_width); // Horizontal offset for each square

            // Choose color based on the square position
            if ((y + x) % 2 == 0)
                attron(COLOR_PAIR(3)); // Light square
            else
                attron(COLOR_PAIR(4)); // Dark square

            // Draw the square borders
            mvprintw(y_offset, x_offset, "+----+");
            mvprintw(y_offset + 1, x_offset, "|    |");

            mvprintw(y_offset + 2, x_offset, "|    |");
            // Display the piece inside the square
            attron(A_BOLD);
            mvaddwstr(y_offset + 2, x_offset + 2, unicode_piece_char(board[y][x]));
            attroff(A_BOLD);

            // Draw the rest of the square
            mvprintw(y_offset + 3, x_offset, "|    |");
            mvprintw(y_offset + 4, x_offset, "+----+");

            // Turn off square background colors
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(4));
        }
    }
}

void display_info(int turn)
{
    int info_width = 40;                    // Set an approximate width for the info block
    int offset_x = (COLS - info_width) / 2; // Center it on the left side of the screen (1/4 of the screen width)
    int offset_y = 2;                       // Vertical position

    // Display current player with bold text
    mvprintw(offset_y, offset_x, "Current Player: ");
    attron(A_BOLD); // Turn on bold text
    if (turn == WHITE)
    {
        printw("WHITE");
    }
    else
    {
        printw("BLACK");
    }
    attroff(A_BOLD); // Turn off bold text

    attron(A_ITALIC); // Italic text (if supported)
    mvprintw(offset_y + 2, offset_x, "Enter your move as chess notation (e.g., e2e4).");
    mvprintw(offset_y + 3, offset_x + 1, "('q' to quit)");
    attroff(A_ITALIC); // Turn off italic

    mvprintw(offset_y + 5, offset_x, "Your move: ");
}