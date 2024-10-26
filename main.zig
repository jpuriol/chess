const std = @import("std");

const display = @import("display.zig");
const logic = @import("logic.zig");

pub fn main() void {
    _ = logic.init();

    display.init();
    defer display.close();

    display.clear();

    _ = display.readLine();
}
