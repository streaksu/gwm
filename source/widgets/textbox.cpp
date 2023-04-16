//  textbox.cpp: Textbox widgets.
//  Copyright (C) 2023 streaksu
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <widgets/textbox.h>
#include <font.h>
#include <stdlib.h>
#include <string.h>

#define TITLEBAR_FONT_COLOR     0x2E3436
#define WINDOW_BACKGROUND_COLOR 0xF0F0F0

struct textbox *init_textbox(const char *text) {
    struct textbox *ret = new struct textbox;
    ret->text = text;
    return ret;
}

void draw_textbox(struct textbox *tx, struct framebuffer *fb, int start_x,
    int start_y, int width_x, int width_y, int center)
{
    const size_t char_count    = strlen(tx->text);
    const size_t char_per_line = width_x / 8;
    const size_t line_count    = width_y / 16;

    size_t last_start        = 0;
    size_t current_line_char = 0;
    size_t current_line      = 0;
    for (size_t i = 0; i < char_count; i++) {
        if (current_line == line_count) {
            return;
        }
        if (current_line_char == char_per_line) {
            draw_string(fb, start_x, start_y + (current_line * 16), tx->text + last_start, char_per_line, TITLEBAR_FONT_COLOR, WINDOW_BACKGROUND_COLOR);
            current_line_char = 0;
            current_line += 1;
            last_start = i;
        } else {
            current_line_char++;
        }

        if (tx->text[i] == '\n' && last_start == i) {
            current_line_char = 0;
            current_line += 1;
            last_start = i + 1;
        } else if (tx->text[i] == '\n') {
            size_t line_start_x;
            if (center) {
                line_start_x = (width_x / 2) - (((i - last_start) / 2) * 8);
            } else {
                line_start_x = 0;
            }
            line_start_x += start_x;
            draw_string(fb, line_start_x, start_y + (current_line * 16), tx->text + last_start, i - last_start, TITLEBAR_FONT_COLOR, WINDOW_BACKGROUND_COLOR);
            current_line_char = 0;
            current_line += 1;
            last_start = i + 1;
        }
    }

    size_t line_start_x;
    if (center) {
        line_start_x = (width_x / 2) - (((char_count - last_start) / 2) * 8);
    } else {
        line_start_x = 0;
    }
    line_start_x += start_x;
    draw_string(fb, line_start_x, start_y + (current_line * 16), tx->text + last_start, char_count - last_start, TITLEBAR_FONT_COLOR, WINDOW_BACKGROUND_COLOR);
}

void destroy_textbox(struct textbox *tx) {
    free(tx);
}
