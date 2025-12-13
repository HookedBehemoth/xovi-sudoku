// Imhex pattern file for the sudoku
// apparently there are no bitfield arrays in imhex but hey,
// the docs have AI!!!
import std.sys;
import std.core;

struct Header {
    char magic[8];
    u32 length;
};

bitfield CellValue {
    lo : 4;
    hi : 4;
};

struct Puzzle {
    CellValue values[41];
    u8 mask[11];
};

Header head @ 0x00;
Puzzle puzzles[head.length] @ 0xc;