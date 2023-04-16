//  window.cpp: Window interface.
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

#include <window.h>
#include <stdlib.h>
#include <string.h>
#include <font.h>
#include <fb.h>

#define TITLEBAR_FOCUS_FONT_COLOR    0x8F97D6
#define TITLEBAR_NO_FOCUS_FONT_COLOR 0xFFFFFF
#define TITLEBAR_COLOR               0x444444
#define WINDOW_BACKGROUND_COLOR      0xF0F0F0
#define WINDOW_BORDERS_COLOR         0xffffff

struct window *create_window(const char *name) {
    struct window *ret = new struct window;
    strncpy(ret->name + 1, name, WINDOW_NAME_LEN - 2);
    ret->name[0] = ' ';
    ret->is_focus     = 0;
    ret->top_corner_x = 100;
    ret->top_corner_y = 100;
    ret->length_x     = 500;
    ret->length_y     = 300;
    memset(ret->children, 0, sizeof(ret->children));
    return ret;
}

void add_child(struct window *win, struct widget *wid) {
    for (int i = 0; i < WINDOW_CHILDREN_LEN; i++) {
        if (win->children[i] == NULL) {
            win->children[i] = wid;
            return;
        }
    }
}

enum window_click_action click_window(struct window *win, int x, int y) {
    if (x >= win->top_corner_x && x <= win->top_corner_x + win->length_x &&
        y >= win->top_corner_y)
    {
        if (y <= win->top_corner_y + 16) {
            return WINDOW_BAR_CLICK;
        } else if (y <= win->top_corner_y + win->length_y) {
            return WINDOW_CONTENT_CLICK;
        }
    }

    return WINDOW_NOT_TOUCHED;
}

void move_window(struct window *win, int x_variation, int y_variation,
    int start_x, int start_y, int max_x, int max_y)
{
    win->top_corner_x += x_variation;
    win->top_corner_y += y_variation;

    if (win->top_corner_x + win->length_x < start_x + 20) {
        win->top_corner_x = start_x + 20 - win->length_x;
    } else if (win->top_corner_x + 20 > max_x) {
        win->top_corner_x = max_x - 20;
    }

    if (win->top_corner_y <= start_y + 16) {
        win->top_corner_y = start_y + 16;
    } else if (win->top_corner_y > max_y - 16) {
        win->top_corner_y = max_y - 16;
    }
}

void focus_window(struct window *win) {
    win->is_focus = 1;
}

void unfocus_window(struct window *win) {
    win->is_focus = 0;
}

void draw_window(struct window *win, struct framebuffer *fb) {
    const int start_x      = win->top_corner_x;
    const int final_x      = win->top_corner_x + win->length_x;
    const int start_y      = win->top_corner_y;
    const int start_body_y = win->top_corner_y + 16;
    const int final_y      = win->top_corner_y + win->length_y;

    // Draw the window bar.
    draw_rectangle(fb, start_x, start_y, final_x, start_body_y, TITLEBAR_COLOR);

    // Draw the rest of the window background.
    draw_rectangle(fb, start_x, start_body_y, final_x, final_y,
        WINDOW_BACKGROUND_COLOR);

    // Draw window title.
    size_t char_count = strlen(win->name);
    const uint32_t text_color = win->is_focus ?
      TITLEBAR_FOCUS_FONT_COLOR : TITLEBAR_NO_FOCUS_FONT_COLOR;
    if (char_count * 8 > (size_t)win->length_x) {
        char_count = win->length_x / 8;
    }
    std::string str(win->name, char_count);
    draw_str(fb, start_x, start_y, str, text_color, TITLEBAR_COLOR, 1);

    // Draw borders.
    for (int i = 0; i <= win->length_y; i++) {
        draw_pixel(fb, start_x, start_y + i, WINDOW_BORDERS_COLOR);
        draw_pixel(fb, final_x, start_y + i, WINDOW_BORDERS_COLOR);
    }
    for (int i = 0; i < win->length_x; i++) {
        draw_pixel(fb, start_x + i, start_y,               WINDOW_BORDERS_COLOR);
        draw_pixel(fb, start_x + i, start_y + 16, WINDOW_BORDERS_COLOR);
        draw_pixel(fb, start_x + i, final_y,               WINDOW_BORDERS_COLOR);
    }

    // Count the children so we can divide the window real state in rows.
    int child_count = 0;
    for (int i = 0; i < WINDOW_CHILDREN_LEN; i++) {
        if (win->children[i] != NULL) {
            child_count++;
        }
    }
    if (child_count == 0) {
        return;
    }

    const size_t step   = win->length_y / child_count;
    size_t current_y    = start_body_y;
    size_t taken_away_y = 0;
    for (int i = 0; i < WINDOW_CHILDREN_LEN; i++) {
        if (win->children[i] != NULL) {
            draw_widget(win->children[i], fb, start_x, current_y,
                win->length_x, step + taken_away_y);
            current_y += step;
        }
    }
}

void destroy_window(struct window *win) {
    for (int i = 0; i < WINDOW_CHILDREN_LEN; i++) {
        if (win->children[i] != NULL) {
            destroy_widget(win->children[i]);
        }
    }

    free(win);
}
