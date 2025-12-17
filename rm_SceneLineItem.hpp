#pragma once

#include "rm_Line.hpp"
#include "rm_SceneItem.hpp"

struct SceneLineItem : public SceneItem {
    // /* 0x00 */ void* vtable;
    /* 0x04 */ unsigned char unk_x4;
    /* 0x08 */ int pageIndex; // starts at 0xE?
    /* 0x0c */ short unk_xc; // always zero?
    /* 0x0e */ short unk_xe; // always one?
    /* 0x10 */ int sourceLayerId; // always layer1 = 0xB, otherwise arbitrary?
    /* 0x14 */ short unk_x14; // always zero?
    /* 0x16 */ short unk_x16; // non-zero if copied from another layer?
    /* 0x18 */ int unk_x18[2];
    /* 0x20 */ unsigned char unk_x20; // 0 or 2
    /* 0x21 */ unsigned char unk_x21; // 0 or 2
    /* 0x24 */ int unk_x24[3];
    /* 0x30 */ Line line;
    /* 0x38 */ int unk_x78; // always 1
    /* 0x3c */ int unk_x7c[3];

    static void* vtable_ptr;
    static void setupVtable(void* vtable);

    static SceneLineItem fromLine(Line &&line) {
        SceneLineItem item = {};
        item.vtable = vtable_ptr;
        item.unk_x4 = 3;
        item.pageIndex = 0xE;
        item.unk_xe = 1;
        item.sourceLayerId = 0xB;
        item.line = line;
        item.unk_x78 = 1;
        return item;
    }

    static void log(const SceneLineItem& item);
};
static_assert(sizeof(SceneLineItem) == 0x88);
