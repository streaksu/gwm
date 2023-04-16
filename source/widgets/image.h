//  image.h: Image widgets.
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

#include <fb.h>
#include <png.h>

struct image {
    png_bytep *image_rows;
    int        image_width;
    int        image_height;
    png_byte   image_color_type;
};

struct image *init_image(const char *path);
void draw_image(struct image *im, struct framebuffer *fb, int start_x,
    int start_y, int width_x, int width_y, int center);
void destroy_image(struct image *im);
