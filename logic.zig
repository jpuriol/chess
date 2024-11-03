pub const Piece = struct {
    type: PieceType,
    player: Player,
};

pub const Player = enum { white, black };

pub const PieceType = enum {
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

    pub fn move(self: *Game, m: Move) MoveError!void {
        const piece = self.board[m.sy][m.sx] orelse {
            return MoveError.NoPieceAtSource;
        };

        if (piece.player != self.turn) {
            return MoveError.WrongPlayerTurn;
        }

        self.board[m.dy][m.dx] = piece;
        self.board[m.sy][m.sx] = null;

        self.turn = switch (self.turn) {
            .white => .black,
            .black => .white,
        };
    }
};

pub fn init() Game {
    const boardSize = Game.boardSize;

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

    // Place pawns
    x = 0;
    while (x < boardSize) : (x += 1) {
        game.board[1][x] = Piece{ .player = .black, .type = .pawn };
        game.board[6][x] = Piece{ .player = .white, .type = .pawn };
    }

    // Place rooks
    game.board[0][0] = Piece{ .player = .black, .type = .rook };
    game.board[0][7] = Piece{ .player = .black, .type = .rook };
    game.board[7][0] = Piece{ .player = .white, .type = .rook };
    game.board[7][7] = Piece{ .player = .white, .type = .rook };

    // Place knights
    game.board[0][1] = Piece{ .player = .black, .type = .knight };
    game.board[0][6] = Piece{ .player = .black, .type = .knight };
    game.board[7][1] = Piece{ .player = .white, .type = .knight };
    game.board[7][6] = Piece{ .player = .white, .type = .knight };

    // Place bishops
    game.board[0][2] = Piece{ .player = .black, .type = .bishop };
    game.board[0][5] = Piece{ .player = .black, .type = .bishop };
    game.board[7][2] = Piece{ .player = .white, .type = .bishop };
    game.board[7][5] = Piece{ .player = .white, .type = .bishop };

    // Place queens
    game.board[0][3] = Piece{ .player = .black, .type = .queen };
    game.board[7][3] = Piece{ .player = .white, .type = .queen };

    // Place kings
    game.board[0][4] = Piece{ .player = .black, .type = .king };
    game.board[7][4] = Piece{ .player = .white, .type = .king };

    return game;
}

pub const MoveError = error{
    InvalidNotation,
    NoPieceAtSource,
    WrongPlayerTurn,
};

pub const Move = struct {
    sx: u8,
    sy: u8,
    dx: u8,
    dy: u8,

    pub fn parse(input: []const u8) MoveError!Move {
        if (input.len != 4) {
            return MoveError.InvalidNotation;
        }

        const sx = try parseFile(input[0]);
        const sy = try parseRank(input[1]);
        const dx = try parseFile(input[2]);
        const dy = try parseRank(input[3]);

        return Move{
            .sx = sx,
            .sy = sy,
            .dx = dx,
            .dy = dy,
        };
    }
};

/// Converts a file character (e.g., 'a' to 'h') to a column index (0-based).
fn parseFile(c: u8) MoveError!u8 {
    if (c >= 'a' and c <= 'h') {
        return c - 'a';
    }
    return MoveError.InvalidNotation;
}

/// Converts a rank character (e.g., '1' to '8') to a row index (0-based).
fn parseRank(c: u8) MoveError!u8 {
    if (c >= '1' and c <= '8') {
        return '8' - c;
    }
    return MoveError.InvalidNotation;
}
