#ifndef _GAME_DEFS_H_
#define _GAME_DEFS_H_

#define BOARD_SIZE 8

enum Players
{
    NONE,
    WHITE,
    BLACK
};

typedef struct
{
    char type;
    int player;
} Piece;

enum PieceTypes
{
    EMPTY = ' ',
    PAWN = 'P',
    ROOK = 'R',
    KNIGHT = 'N',
    BISHOP = 'B',
    QUEEN = 'Q',
    KING = 'K'
};

#endif