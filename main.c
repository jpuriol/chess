#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

// Board size
#define BOARD_SIZE 8

// Piece types
enum Pieces { EMPTY = ' ', PAWN = 'P', ROOK = 'R', KNIGHT = 'N', BISHOP = 'B', QUEEN = 'Q', KING = 'K' };

// Players
enum Players { NONE, WHITE, BLACK };

// Structure to represent a piece
typedef struct {
    char type;
    int player;
} Piece;

// Global Variables
Piece board[BOARD_SIZE][BOARD_SIZE];
int turn = WHITE;  // Start with WHITE player

// Function Prototypes
void init_ncurses();
void init_board();
void draw_board();
void close_ncurses();
int move_piece(int sx, int sy, int dx, int dy);
int is_valid_move(int sx, int sy, int dx, int dy);
void display_info();
void parse_input(char *input, int *sx, int *sy, int *dx, int *dy);

int main() {
    init_ncurses();
    init_board();

    char input[6];
    int sx, sy, dx, dy;

    while (1) {
        clear();
        draw_board();              // Draw the board with colored pieces and labels
        display_info();            // Show current player and instructions

        mvprintw(20, 0, "Enter your move (e.g., e2e4): ");
        getstr(input);             // Get the move input from the user

        // Quit the game if the user inputs "q"
        if (strcmp(input, "q") == 0) {
            break;
        }

        // Parse the input into source (sx, sy) and destination (dx, dy)
        parse_input(input, &sx, &sy, &dx, &dy);

        // Attempt the move
        if (move_piece(sx, sy, dx, dy)) {
            turn = (turn == WHITE) ? BLACK : WHITE;  // Switch player after a valid move
        } else {
            mvprintw(22, 0, "Invalid move! Press any key to try again...");
            getch();
        }
    }

    close_ncurses();
    return 0;
}

// Initialize ncurses with color support
void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Initialize colors
    if (has_colors()) {
        start_color();
        // Define color pairs: foreground (text color), background (background color)
        init_pair(1, COLOR_WHITE, COLOR_BLACK);  // White pieces: white text on black
        init_pair(2, COLOR_BLACK, COLOR_WHITE);  // Black pieces: black text on white
    }
}

// Initialize the board with pieces
void init_board() {
    // Initialize empty board
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            board[y][x].type = EMPTY;
            board[y][x].player = NONE;
        }
    }

    // Place pawns
    for (int x = 0; x < BOARD_SIZE; x++) {
        board[1][x].type = PAWN; board[1][x].player = WHITE;
        board[6][x].type = PAWN; board[6][x].player = BLACK;
    }

    // Place rooks
    board[0][0].type = board[0][7].type = ROOK; board[0][0].player = board[0][7].player = WHITE;
    board[7][0].type = board[7][7].type = ROOK; board[7][0].player = board[7][7].player = BLACK;

    // Place knights
    board[0][1].type = board[0][6].type = KNIGHT; board[0][1].player = board[0][6].player = WHITE;
    board[7][1].type = board[7][6].type = KNIGHT; board[7][1].player = board[7][6].player = BLACK;

    // Place bishops
    board[0][2].type = board[0][5].type = BISHOP; board[0][2].player = board[0][5].player = WHITE;
    board[7][2].type = board[7][5].type = BISHOP; board[7][2].player = board[7][5].player = BLACK;

    // Place queens
    board[0][3].type = QUEEN; board[0][3].player = WHITE;
    board[7][3].type = QUEEN; board[7][3].player = BLACK;

    // Place kings
    board[0][4].type = KING; board[0][4].player = WHITE;
    board[7][4].type = KING; board[7][4].player = BLACK;
}

// Draw the board with borders, colored pieces, and row/column labels
void draw_board() {
    // Draw the column labels (a-h)
    mvprintw(1, 16 + 3, "a   b   c   d   e   f   g   h");
    mvprintw(37, 16 + 3, "a   b   c   d   e   f   g   h");

    for (int y = 0; y < BOARD_SIZE; y++) {
        // Draw the row label (1-8) on the left and right side of the board
        mvprintw(y * 4 + 2, 14, "%d", BOARD_SIZE - y);
        mvprintw(y * 4 + 2, 16 + 8 * 6 + 2, "%d", BOARD_SIZE - y);

        for (int x = 0; x < BOARD_SIZE; x++) {
            // Draw the square border, offset by 16 columns to make space for info
            mvprintw(y * 4, x * 6 + 16, "+----+");
            mvprintw(y * 4 + 1, x * 6 + 16, "|    |");

            // Use different color pairs for white and black pieces
            if (board[y][x].player == WHITE) {
                attron(COLOR_PAIR(1));  // White piece color
            } else if (board[y][x].player == BLACK) {
                attron(COLOR_PAIR(2));  // Black piece color
            }

            mvprintw(y * 4 + 2, x * 6 + 16, "| %c  |", board[y][x].type); // Display the piece

            // Turn off color
            attroff(COLOR_PAIR(1));
            attroff(COLOR_PAIR(2));

            mvprintw(y * 4 + 3, x * 6 + 16, "|    |");
            mvprintw(y * 4 + 4, x * 6 + 16, "+----+");
        }
    }
}

// Close ncurses
void close_ncurses() {
    endwin();
}

// Move a piece if valid
int move_piece(int sx, int sy, int dx, int dy) {
    if (is_valid_move(sx, sy, dx, dy)) {
        board[dy][dx] = board[sy][sx];
        board[sy][sx].type = EMPTY;
        board[sy][sx].player = NONE;
        return 1;
    }
    return 0;
}

// Validate the move (simple version)
int is_valid_move(int sx, int sy, int dx, int dy) {
    // Add simple move validation for now (e.g., no out-of-bound or same-player destination)
    if (dx < 0 || dx >= BOARD_SIZE || dy < 0 || dy >= BOARD_SIZE) return 0;
    if (board[sy][sx].player == board[dy][dx].player) return 0;  // Can't move to a spot with the same player

    // Add specific movement rules for each piece here (e.g., pawns move forward, knights move in L shape)
    
    return 1;  // Accepting all moves for now
}

// Parse the user input (e.g., "e2e4") and convert it into board coordinates
void parse_input(char *input, int *sx, int *sy, int *dx, int *dy) {
    if (strlen(input) != 4) {
        *sx = *sy = *dx = *dy = -1;  // Invalid input
        return;
    }

    *sx = input[0] - 'a';  // 'a' -> 0, 'b' -> 1, ..., 'h' -> 7
    *sy = '8' - input[1];  // '8' -> 0, '7' -> 1, ..., '1' -> 7
    *dx = input[2] - 'a';
    *dy = '8' - input[3];
}

// Display current player and instructions
void display_info() {
    // Display which player's turn it is
    mvprintw(2, 0, "Current Player:");
    if (turn == WHITE) {
        mvprintw(3, 0, "WHITE");
    } else {
        mvprintw(3, 0, "BLACK");
    }

    // Show instructions
    mvprintw(5, 0, "Instructions:");
    mvprintw(6, 0, "Enter your move as");
    mvprintw(7, 0, "chess notation (e.g.,");
    mvprintw(8, 0, "'e2e4').");
    mvprintw(10, 0, "'q' to quit");
}
