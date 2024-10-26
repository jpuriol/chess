const ncurses = @cImport({
    @cInclude("ncurses.h");
});

pub fn init() void {
    _ = ncurses.initscr();
    _ = ncurses.cbreak();
    // ncurses.keypad(ncurses.stdscr, ncurses.TRUE);
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
