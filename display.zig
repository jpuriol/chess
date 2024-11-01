const std = @import("std");

const ncurses = @cImport({
    @cInclude("ncurses.h");
});

const locale = @cImport(@cInclude("locale.h"));

const logic = @import("logic.zig");

pub fn init() void {
    _ = ncurses.initscr();
    _ = ncurses.cbreak();
    // ncurses.keypad(ncurses.stdscr, ncurses.TRUE);

    // Set locale for wide characters (must be set before using Unicode)
    _ = locale.setlocale(locale.LC_ALL, "");

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

pub fn readLine() []u8 {
    var len: usize = 0;

    while (len < buffer.len) {
        const ch = ncurses.getch();
        if (ch == '\n') break;

        buffer[len] = @as(u8, @intCast(ch));
        len += 1;
    }

    return buffer[0..len];
}

pub fn draw(game: logic.Game) void {
    const boardSize = logic.Game.boardSize;

    const origin_x = 8;
    const origin_y = 4;
    const tile_width = 8;
    const tile_height = 4;

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
            const color = tileColor(game, @intCast(y), @intCast(x));
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

            if (game.board[@intCast(y)][@intCast(x)]) |p| {
                const piece_y = y_offset + (tile_height / 2);
                const piece_x = x_offset + (tile_width / 2) - 1;

                const piece_ch = char_piece(p);
                const piece_wch = wchar_piece(p);

                _ = ncurses.move(piece_y, piece_x);
                _ = ncurses.addch(piece_ch);
                _ = ncurses.addwstr(piece_wch.ptr);
            }
        }
    }
}

fn tileColor(g: logic.Game, y: usize, x: usize) ColorPair {
    const light = @mod(y + x, 2) == 0;

    // Check if there is a piece at the given position.
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

const wchar = c_int;

fn wchar_piece(piece: logic.Piece) []const wchar {
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

    var wchar_array: [1]wchar = undefined;

    const codepoint = std.unicode.utf8Decode(utf8) catch {
        return wchar_array[0..0]; // Return an empty slice if decoding fails
    };
    wchar_array[0] = codepoint;
    return wchar_array[0..1];
}

fn char_piece(piece: logic.Piece) c_uint {
    return switch (piece.type) {
        .pawn => 'P',
        .rook => 'R',
        .knight => 'N',
        .bishop => 'B',
        .queen => 'Q',
        .king => 'K',
    };
}
