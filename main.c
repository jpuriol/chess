#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

// Board size
#define BOARD_SIZE 8

// Piece types
enum Pieces
{
    EMPTY = ' ',
    PAWN = 'P',
    ROOK = 'R',
    KNIGHT = 'N',
    BISHOP = 'B',
    QUEEN = 'Q',
    KING = 'K'
};

// Players
enum Players
{
    NONE,
    WHITE,
    BLACK
};

// Structure to represent a piece
typedef struct
{
    char type;
    int player;
} Piece;

// Global Variables
Piece board[BOARD_SIZE][BOARD_SIZE];
int turn = WHITE; // Start with WHITE player

// Function Prototypes
void init_ncurses();
void init_board();
void draw_board();
void close_ncurses();
int move_piece(int sx, int sy, int dx, int dy);
int is_valid_move(int sx, int sy, int dx, int dy);
void display_info();
void parse_input(char *input, int *sx, int *sy, int *dx, int *dy);

int main()
{
    init_ncurses();
    init_board();

    char input[6];
    int sx, sy, dx, dy;

    while (1)
    {
        clear();
        draw_board();   // Draw the board with colored pieces and labels
        display_info(); // Show current player and instructions

        getstr(input); // Get the move input from the user

        // Quit the game if the user inputs "q"
        if (strcmp(input, "q") == 0)
        {
            break;
        }

        // Parse the input into source (sx, sy) and destination (dx, dy)
        parse_input(input, &sx, &sy, &dx, &dy);

        // Attempt the move
        if (move_piece(sx, sy, dx, dy))
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

// Initialize ncurses with color support
void init_ncurses()
{
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Initialize colors
    if (has_colors())
    {
        start_color();
        // Define color pairs: foreground (text color), background (background color)
        init_pair(1, COLOR_WHITE, COLOR_BLACK); // White pieces: white text on black
        init_pair(2, COLOR_BLACK, COLOR_WHITE); // Black pieces: black text on white
    }
}

// Initialize the board with pieces
void init_board()
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

void draw_board()
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

            // Draw the square borders
            mvprintw(y_offset, x_offset, "+----+");
            mvprintw(y_offset + 1, x_offset, "|    |");

            // Display the piece inside the square
            mvprintw(y_offset + 2, x_offset, "|");

            // Use different color pairs for white and black pieces
            if (board[y][x].player == WHITE)
            {
                attron(COLOR_PAIR(1)); // White piece color
            }
            else if (board[y][x].player == BLACK)
            {
                attron(COLOR_PAIR(2)); // Black piece color
            }

            printw(" %c ", board[y][x].type); // Print the piece type (e.g., P, R, N)

            // Turn off color
            attroff(COLOR_PAIR(1));
            attroff(COLOR_PAIR(2));

            printw(" |");

            // Draw the rest of the square
            mvprintw(y_offset + 3, x_offset, "|    |");
            mvprintw(y_offset + 4, x_offset, "+----+");
        }
    }
}

// Close ncurses
void close_ncurses()
{
    endwin();
}

// Move a piece if valid
int move_piece(int sx, int sy, int dx, int dy)
{
    if (is_valid_move(sx, sy, dx, dy))
    {
        board[dy][dx] = board[sy][sx];
        board[sy][sx].type = EMPTY;
        board[sy][sx].player = NONE;
        return 1;
    }
    return 0;
}

// Validate the move (simple version)
int is_valid_move(int sx, int sy, int dx, int dy)
{
    // Add simple move validation for now (e.g., no out-of-bound or same-player destination)
    if (dx < 0 || dx >= BOARD_SIZE || dy < 0 || dy >= BOARD_SIZE)
        return 0;
    if (board[sy][sx].player == board[dy][dx].player)
        return 0; // Can't move to a spot with the same player

    // Add specific movement rules for each piece here (e.g., pawns move forward, knights move in L shape)

    return 1; // Accepting all moves for now
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

// Display current player and instructions below the board in the desired format
void display_info()
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
