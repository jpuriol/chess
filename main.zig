const std = @import("std");

const display = @import("display.zig");
const logic = @import("logic.zig");

pub fn main() void {
    const game = logic.init();

    display.init();
    defer display.close();

    display.clear();
    display.draw(game);

    _ = display.readLine();
}
