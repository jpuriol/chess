const Piece = struct {
    type: PieceType,
    player: Player,
};

const Player = enum { white, black };

const PieceType = enum {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
};

const boardSize = 8;

const Game = struct {
    board: [boardSize][boardSize]?Piece,
    turn: Player,
};

pub fn init() Game {
    var game = Game{
        .turn = Player.white, // White always begins
        .board = undefined,
    };

    var x: usize = 0;
    var y: usize = 0;
    // Initialize empty board
    while (y < boardSize) : (y += 1) {
        while (x < boardSize) : (x += 1) {
            game.board[y][x] = null;
        }
    }

    return game;
}
