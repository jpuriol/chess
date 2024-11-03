const std = @import("std");

const ncurses = @cImport({
    @cInclude("ncurses.h");
});

const locale = @cImport(@cInclude("locale.h"));

const logic = @import("logic.zig");

pub fn init() void {
    _ = ncurses.initscr();
    _ = ncurses.cbreak();
    _ = ncurses.noecho();

    // Set locale for wide characters (must be set before using Unicode)
    _ = locale.setlocale(locale.LC_ALL, "en_US.UTF-8");

    _ = ncurses.start_color();

    initColor(.lightGrey, 800, 800, 800);
    initColor(.darkGrey, 50, 50, 50);
    initColor(.lightBrown, 500, 300, 150);
    initColor(.darkBrown, 400, 200, 100);

    initPair(.lightSquareBlack, .darkGrey, .lightBrown);
    initPair(.lightSquareWhite, .lightGrey, .lightBrown);

    initPair(.darkSquareBlack, .darkGrey, .darkBrown);
    initPair(.darkSquareWhite, .lightGrey, .darkBrown);
}

const Color = enum(c_short) {
    lightGrey = 10,
    darkGrey = 11,
    lightBrown = 12,
    darkBrown = 13,
};

const ColorPair = enum(c_short) {
    lightSquareBlack = 1,
    lightSquareWhite = 2,
    darkSquareBlack = 3,
    darkSquareWhite = 4,
};

fn initColor(c: Color, r: comptime_int, g: comptime_int, b: comptime_int) void {
    _ = ncurses.init_color(
        @intFromEnum(c),
        r,
        g,
        b,
    );
}

fn initPair(p: ColorPair, foreground: Color, background: Color) void {
    _ = ncurses.init_pair(
        @intFromEnum(p),
        @intFromEnum(foreground),
        @intFromEnum(background),
    );
}

pub fn close() void {
    _ = ncurses.endwin();
}

pub fn clear() void {
    _ = ncurses.clear();
}

pub fn waitForKey() void {
    _ = ncurses.getch();
}

var buffer: [32]u8 = undefined;

pub fn readInput() []const u8 {
    var len: usize = 0;

    const line_y = info_y + 5;
    _ = ncurses.move(line_y, info_x);

    while (len < buffer.len) : (_ = ncurses.refresh()) {
        const ch = @as(u8, @intCast(ncurses.getch()));
        if (ch == '\n') break;

        if (ch == std.ascii.control_code.del) {
            if (len > 0) {
                // Move the cursor back and delete the character
                const x = info_x + len;
                _ = ncurses.move(line_y, @intCast(x - 1));
                _ = ncurses.addch(' ');
                _ = ncurses.move(line_y, @intCast(x - 1));

                // Decrement length
                len -= 1;
            }
            continue; // Skip adding character to buffer
        }

        if (!std.ascii.isAlphanumeric(ch)) {
            continue; // Skip adding character to buffer
        }

        _ = ncurses.addch(ch);
        buffer[len] = ch;
        len += 1;
    }

    return buffer[0..len];
}

pub fn informInvalidMove() void {
    const line_y = info_y + 7;

    _ = ncurses.mvprintw(line_y, info_x, "Invalid move! Press any key to try again...");
    _ = ncurses.getch();
}

// Basic parameters
const origin_x = 8;
const origin_y = 2;

// Board parameters
const tile_width = 11;
const tile_height = 5;

// Information section parameters
const info_x = origin_x + (tile_width * logic.Game.boardSize) + 15;
const info_y = origin_y + ((tile_height * logic.Game.boardSize) / 2) - 4;

pub fn draw(game: logic.Game) void {
    drawBoard(game);

    _ = ncurses.mvprintw(info_y, info_x, "Current player: ");
    const color = switch (game.turn) {
        .white => ColorPair.darkSquareWhite,
        .black => ColorPair.darkSquareBlack,
    };
    const color_id = @intFromEnum(color);
    _ = ncurses.attron(ncurses.COLOR_PAIR(color_id));
    switch (game.turn) {
        .white => {
            _ = ncurses.printw(" WHITE ");
        },
        .black => {
            _ = ncurses.printw(" BLACK ");
        },
    }
    _ = ncurses.attroff(ncurses.COLOR_PAIR(color_id));

    _ = ncurses.mvprintw(info_y + 2, info_x, "Enter your move as chess notation (e.g., e2e4).");
    _ = ncurses.mvprintw(info_y + 3, info_x, "('q' to quit)");
}

fn drawBoard(state: logic.Game) void {
    const boardSize = logic.Game.boardSize;

    // Draw the column labels (a-h)
    var x: c_int = 0;
    while (x < boardSize) : (x += 1) {
        const column_label = 'a' + x;

        const column_center = tile_width / 2;
        const column_label_x = origin_x + (x * tile_width) + column_center;
        const column_label_y_top = origin_y - 1;
        const column_label_y_bottom = origin_y + (boardSize * tile_height);

        _ = ncurses.mvprintw(column_label_y_top, column_label_x, "%c", column_label);
        _ = ncurses.mvprintw(column_label_y_bottom, column_label_x, "%c", column_label);
    }

    // Draw the row labels (1-8)
    var y: c_int = 0;
    while (y < boardSize) : (y += 1) {
        const row_label = boardSize - y;

        const row_center = tile_height / 2;
        const row_label_y = origin_y + (y * tile_height) + row_center;
        const row_label_x_left = origin_x - 2;
        const row_label_y_right = origin_x + (boardSize * tile_width) + 1;

        _ = ncurses.mvprintw(row_label_y, row_label_x_left, "%d", row_label);
        _ = ncurses.mvprintw(row_label_y, row_label_y_right, "%d", row_label);
    }

    // Draw the board
    y = 0;
    while (y < boardSize) : (y += 1) {
        const y_offset = origin_y + (y * tile_height);

        x = 0;
        while (x < boardSize) : (x += 1) {
            const color = tileColor(state, @intCast(y), @intCast(x));
            const color_id = @intFromEnum(color);
            _ = ncurses.attron(ncurses.COLOR_PAIR(color_id));
            defer _ = ncurses.attroff(ncurses.COLOR_PAIR(color_id));

            const x_offset = origin_x + (x * tile_width);

            // Draw the square
            var i: c_int = 0;
            while (i < tile_height) : (i += 1) {
                _ = ncurses.move(y_offset + i, x_offset);
                var j: c_int = 0;
                while (j < tile_width) : (j += 1) {
                    _ = ncurses.addch(' ');
                }
            }

            if (state.board[@intCast(y)][@intCast(x)]) |p| {
                const piece_y = y_offset + (tile_height / 2);
                const piece_x = x_offset + (tile_width / 2);

                const piece_ch = charPiece(p);
                const piece_wch = &[_:0]c_int{wcharPiece(p)};

                _ = ncurses.move(piece_y, piece_x);
                _ = ncurses.addch(piece_ch);
                _ = ncurses.addwstr(piece_wch.ptr);
            }
        }
    }
}

fn tileColor(g: logic.Game, y: usize, x: usize) ColorPair {
    const light = @mod(y + x, 2) == 0;

    if (g.board[y][x]) |p| {
        // If there's a piece, return the appropriate color based on the player and light/dark status.
        return switch (p.player) {
            .white => if (light) ColorPair.lightSquareWhite else ColorPair.darkSquareWhite,
            .black => if (light) ColorPair.lightSquareBlack else ColorPair.darkSquareBlack,
        };
    } else {
        // If there's no piece, return the color for an empty tile based on the light/dark status.
        return if (light) ColorPair.lightSquareBlack else ColorPair.darkSquareBlack;
    }
}

fn wcharPiece(piece: logic.Piece) c_int {
    const utf8 = switch (piece.player) {
        .white => switch (piece.type) {
            .pawn => "♙",
            .rook => "♖",
            .knight => "♘",
            .bishop => "♗",
            .queen => "♕",
            .king => "♔",
        },
        .black => switch (piece.type) {
            .pawn => "♟",
            .rook => "♜",
            .knight => "♞",
            .bishop => "♝",
            .queen => "♛",
            .king => "♚",
        },
    };

    const codepoint = std.unicode.utf8Decode(utf8) catch unreachable;
    // const wch: c_int = codepoint;
    return codepoint;
}

fn charPiece(piece: logic.Piece) c_uint {
    return switch (piece.type) {
        .pawn => 'P',
        .rook => 'R',
        .knight => 'N',
        .bishop => 'B',
        .queen => 'Q',
        .king => 'K',
    };
}
