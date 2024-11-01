const ncurses = @cImport({
    @cInclude("ncurses.h");
});

const logic = @import("logic.zig");

pub fn init() void {
    _ = ncurses.initscr();
    _ = ncurses.cbreak();
    // ncurses.keypad(ncurses.stdscr, ncurses.TRUE);

    _ = ncurses.start_color();

    initColor(.lightGrey, 800, 800, 800);
    initColor(.darkGrey, 50, 50, 50);
    initColor(.lightBrown, 500, 300, 150);
    initColor(.darkBrown, 400, 200, 100);

    initPair(.lightSquareBlack, .darkGrey, .lightBrown);
    initPair(.lightSquareWhite, .lightGrey, .darkBrown);

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
    _ = game;

    const origin_x = 8;
    const origin_y = 2;
    const tile_width = 6;
    const tile_height = 4;

    // Draw the column labels (a-h)
    var x: c_int = 0;
    while (x < logic.boardSize) : (x += 1) {
        const column_label = 'a' + x;

        const column_center = tile_width / 2;
        const column_label_x = origin_x + (x * tile_width) + column_center;
        const column_label_y_top = 1;
        const column_label_y_bottom = 35; // TODO: calculate this value

        _ = ncurses.mvprintw(column_label_y_top, column_label_x, "%c", column_label);
        _ = ncurses.mvprintw(column_label_y_bottom, column_label_x, "%c", column_label);
    }

    // Draw the row labels (1-8)
    var y: c_int = 0;
    while (y < logic.boardSize) : (y += 1) {
        const row_label = logic.boardSize - y;

        const row_center = tile_height / 2;
        const row_label_y = origin_y + (y * tile_height) + row_center;
        const row_label_offset = 2;
        const row_label_x_left = origin_x - row_label_offset;
        const row_label_y_right = origin_x + (logic.boardSize * tile_width) + row_label_offset;

        _ = ncurses.mvprintw(row_label_y, row_label_x_left, "%d", row_label);
        _ = ncurses.mvprintw(row_label_y, row_label_y_right, "%d", row_label);
    }

    // Draw the board
    y = 0;
    while (y < logic.boardSize) : (y += 1) {
        const y_offset = origin_y + (y * tile_height);

        x = 0;
        while (x < logic.boardSize) : (x += 1) {
            const color_pair = if (@mod(y + x, 2) == 0)
                ColorPair.lightSquareBlack
            else
                ColorPair.darkSquareBlack;

            const square_color = @intFromEnum(color_pair);
            _ = ncurses.attron(ncurses.COLOR_PAIR(square_color));
            defer _ = ncurses.attroff(ncurses.COLOR_PAIR(square_color));

            const x_offset = origin_x + (x * tile_width);

            _ = ncurses.mvprintw(y_offset + 0, x_offset, "      ");
            _ = ncurses.mvprintw(y_offset + 1, x_offset, "      ");
            _ = ncurses.mvprintw(y_offset + 2, x_offset, "      ");
            _ = ncurses.mvprintw(y_offset + 3, x_offset, "      ");
        }
    }
}
