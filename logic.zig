const Piece = struct {
    type: PieceType,
    player: Player,
};

const Player = enum { white, black };

const PieceType = enum {
    pawn,
    rook,
    knight,
    bishop,
    queen,
    king,
};

pub const Game = struct {
    pub const boardSize = 8;

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
    while (y < Game.boardSize) : (y += 1) {
        while (x < Game.boardSize) : (x += 1) {
            game.board[y][x] = null;
        }
    }

    return game;
}
