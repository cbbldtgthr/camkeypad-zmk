// A 4x7 pixel font on a 5x8 grid, for body text on the OLED: 25 columns at full width.
// LVGL's smallest built-in bitmap font is 8 pixels wide. Each glyph below is seven rows, top
// first, as one hex digit per row, 8 being the left pixel. Capitals fill six rows; the seventh
// is for descenders.

#include <lvgl.h>
#include <zephyr/sys/util.h>

// LVGL packs a glyph's pixels with no padding between rows, here 28 bits in four bytes.
#define G(a, b, c, d, e, f, g)                                                                     \
    0x##a##b, 0x##c##d, 0x##e##f, 0x##g##0

static const uint8_t glyph_bitmap[] = {
    G(0, 0, 0, 0, 0, 0, 0), // space
    G(4, 4, 4, 4, 0, 4, 0), // !
    G(A, A, 0, 0, 0, 0, 0), // "
    G(A, F, A, F, A, 0, 0), // #
    G(4, 7, C, 6, 3, E, 4), // $
    G(9, 1, 2, 4, 8, 9, 0), // %
    G(4, A, A, 4, A, 5, 0), // &
    G(4, 4, 0, 0, 0, 0, 0), // '
    G(2, 4, 4, 4, 4, 2, 0), // (
    G(4, 2, 2, 2, 2, 4, 0), // )
    G(0, 9, 6, F, 6, 9, 0), // *
    G(0, 4, 4, E, 4, 4, 0), // +
    G(0, 0, 0, 0, 6, 2, 4), // ,
    G(0, 0, 0, F, 0, 0, 0), // -
    G(0, 0, 0, 0, 6, 6, 0), // .
    G(1, 1, 2, 4, 8, 8, 0), // /
    G(6, 9, B, D, 9, 6, 0), // 0
    G(2, 6, 2, 2, 2, 7, 0), // 1
    G(6, 9, 1, 2, 4, F, 0), // 2
    G(F, 1, 6, 1, 9, 6, 0), // 3
    G(2, 6, A, F, 2, 2, 0), // 4
    G(F, 8, E, 1, 9, 6, 0), // 5
    G(6, 8, E, 9, 9, 6, 0), // 6
    G(F, 1, 2, 2, 4, 4, 0), // 7
    G(6, 9, 6, 9, 9, 6, 0), // 8
    G(6, 9, 9, 7, 1, 6, 0), // 9
    G(0, 6, 6, 0, 6, 6, 0), // :
    G(0, 6, 6, 0, 6, 2, 4), // ;
    G(1, 2, 4, 4, 2, 1, 0), // <
    G(0, 0, F, 0, F, 0, 0), // =
    G(8, 4, 2, 2, 4, 8, 0), // >
    G(6, 9, 1, 2, 0, 2, 0), // ?
    G(6, 9, B, B, 8, 6, 0), // @
    G(6, 9, 9, F, 9, 9, 0), // A
    G(E, 9, E, 9, 9, E, 0), // B
    G(6, 9, 8, 8, 9, 6, 0), // C
    G(E, 9, 9, 9, 9, E, 0), // D
    G(F, 8, E, 8, 8, F, 0), // E
    G(F, 8, E, 8, 8, 8, 0), // F
    G(6, 9, 8, B, 9, 6, 0), // G
    G(9, 9, F, 9, 9, 9, 0), // H
    G(E, 4, 4, 4, 4, E, 0), // I
    G(7, 2, 2, 2, A, 4, 0), // J
    G(9, A, C, C, A, 9, 0), // K
    G(8, 8, 8, 8, 8, F, 0), // L
    G(9, F, F, 9, 9, 9, 0), // M
    G(9, D, D, B, B, 9, 0), // N
    G(6, 9, 9, 9, 9, 6, 0), // O
    G(E, 9, 9, E, 8, 8, 0), // P
    G(6, 9, 9, 9, D, 6, 1), // Q
    G(E, 9, 9, E, A, 9, 0), // R
    G(6, 9, 4, 2, 9, 6, 0), // S
    G(E, 4, 4, 4, 4, 4, 0), // T
    G(9, 9, 9, 9, 9, 6, 0), // U
    G(9, 9, 9, 9, 6, 6, 0), // V
    G(9, 9, 9, F, F, 9, 0), // W
    G(9, 9, 6, 6, 9, 9, 0), // X
    G(A, A, A, 4, 4, 4, 0), // Y
    G(F, 1, 2, 4, 8, F, 0), // Z
    G(7, 4, 4, 4, 4, 7, 0), // [
    G(8, 8, 4, 2, 1, 1, 0), // backslash
    G(E, 2, 2, 2, 2, E, 0), // ]
    G(4, A, 0, 0, 0, 0, 0), // ^
    G(0, 0, 0, 0, 0, 0, F), // _
    G(4, 2, 0, 0, 0, 0, 0), // `
    G(0, 0, 7, 9, 9, 7, 0), // a
    G(8, 8, E, 9, 9, E, 0), // b
    G(0, 0, 7, 8, 8, 7, 0), // c
    G(1, 1, 7, 9, 9, 7, 0), // d
    G(0, 0, 6, F, 8, 7, 0), // e
    G(2, 5, 4, E, 4, 4, 0), // f
    G(0, 0, 7, 9, 7, 1, 6), // g
    G(8, 8, E, 9, 9, 9, 0), // h
    G(4, 0, C, 4, 4, E, 0), // i
    G(2, 0, 2, 2, 2, A, 4), // j
    G(8, 8, A, C, A, 9, 0), // k
    G(C, 4, 4, 4, 4, E, 0), // l
    G(0, 0, A, F, 9, 9, 0), // m
    G(0, 0, E, 9, 9, 9, 0), // n
    G(0, 0, 6, 9, 9, 6, 0), // o
    G(0, 0, E, 9, E, 8, 8), // p
    G(0, 0, 7, 9, 7, 1, 1), // q
    G(0, 0, B, C, 8, 8, 0), // r
    G(0, 0, 7, C, 3, E, 0), // s
    G(4, 4, E, 4, 4, 3, 0), // t
    G(0, 0, 9, 9, 9, 7, 0), // u
    G(0, 0, 9, 9, 6, 6, 0), // v
    G(0, 0, 9, 9, F, 6, 0), // w
    G(0, 0, 9, 6, 6, 9, 0), // x
    G(0, 0, 9, 9, 7, 1, 6), // y
    G(0, 0, F, 2, 4, F, 0), // z
    G(3, 4, 8, 4, 4, 3, 0), // {
    G(4, 4, 4, 4, 4, 4, 4), // |
    G(C, 2, 1, 2, 2, C, 0), // }
    G(5, A, 0, 0, 0, 0, 0), // ~
};

// adv_w is in sixteenths of a pixel. Glyph id 0 is reserved by LVGL.
#define GLYPH_DSC(i, _)                                                                            \
    {.bitmap_index = (i) * 4, .adv_w = 5 * 16, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0}

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {0},
    LISTIFY(95, GLYPH_DSC, (, ), _),
};

static const lv_font_fmt_txt_cmap_t cmaps[] = {
    {
        .range_start = ' ',
        .range_length = 95,
        .glyph_id_start = 1,
        .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY,
    },
};

static const lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .cmap_num = 1,
    .bpp = 1,
};

const lv_font_t camkeypad_font_small = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,
    .line_height = 8,
    .base_line = 0,
    .subpx = LV_FONT_SUBPX_NONE,
    .dsc = &font_dsc,
};
