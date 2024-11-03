const std = @import("std");

const display = @import("display.zig");
const logic = @import("logic.zig");

pub fn main() void {
    const game = logic.init();

    display.init();
    defer display.close();

    while (true) {
        display.clear();
        display.draw(game);

        const input = display.readLine();

        if (std.mem.eql(u8, input, "q")) {
            break;
        }
    }
}
