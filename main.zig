const std = @import("std");

const display = @import("display.zig");
const logic = @import("logic.zig");

pub fn main() void {
    var game = logic.init();

    display.init();
    defer display.close();

    while (true) {
        display.clear();
        display.draw(game);

        const input = display.readInput();

        if (std.mem.eql(u8, input, "q")) {
            break;
        }

        const m = logic.Move.parse(input) catch {
            display.informInvalidMove();
            continue;
        };

        game.move(m);
    }
}
