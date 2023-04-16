//  fb.h: Framebuffer management.
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

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <widgets/image.h>

struct framebuffer {
    int       backing_fd;
    struct image* background;
    uint32_t *antibuffer;
    uint32_t *frontbuffer;
    uint32_t *memory_window;
    size_t    pixel_width;
    size_t    pixel_height;
    size_t    pitch;
    size_t    pixel_size;
    size_t    linear_size;
};

struct framebuffer *create_framebuffer_from_fd(int fd);
void refresh_to_backend(struct framebuffer *fb);
void draw_background(struct framebuffer *fb);
void draw_pixel(struct framebuffer *fb, int x, int y, uint32_t color);
uint32_t get_pixel(struct framebuffer *fb, int x, int y);
void draw_rectangle(struct framebuffer *fb, int x, int y, int x_end, int y_end, uint32_t color);
void draw_string(struct framebuffer *fb, int x, int y, const char *str, size_t char_count, uint32_t fg, uint32_t bg);
