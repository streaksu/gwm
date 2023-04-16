//  image.cpp: Image widgets.
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

#include <widgets/image.h>
#include <stdlib.h>
#include <png.h>

struct image *init_image(const char *path) {
   char header[8];
   FILE *fp = fopen(path, "rb");
   if (fp == NULL) {
      return NULL;
   }
   fread(header, 1, 8, fp);
   if (png_sig_cmp(reinterpret_cast<png_const_bytep>(&header[0]), 0, 8)) {
      return NULL;
   }

   png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png == NULL) {
      return NULL;
   }
   png_infop pnginfo = png_create_info_struct(png);
   if (pnginfo == NULL) {
      return NULL;
   }

   if (setjmp(png_jmpbuf(png))) {
      return NULL;
   }

   png_init_io(png, fp);
   png_set_sig_bytes(png, 8);
   png_read_info(png, pnginfo);
   int width  = png_get_image_width(png, pnginfo);
   int height = png_get_image_height(png, pnginfo);
   png_byte color_type = png_get_color_type(png, pnginfo);
   png_byte bit_depth  = png_get_bit_depth(png, pnginfo);

   // Transform all colors to RGBA.
   if (bit_depth == 16)
      png_set_strip_16(png);

   if (color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
   png_set_expand_gray_1_2_4_to_8(png);

  if (png_get_valid(png, pnginfo, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

   png_read_update_info(png, pnginfo);

   if (setjmp(png_jmpbuf(png))) {
      return NULL;
   }

   png_bytep *row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
   for (int y = 0; y < height; y++) {
      row_pointers[y] = new png_byte[png_get_rowbytes(png, pnginfo)];
   }
   png_read_image(png, row_pointers);
   fclose(fp);

   struct image *result     = new struct image;
   result->image_rows       = row_pointers;
   result->image_width      = width;
   result->image_height     = height;
   result->image_color_type = color_type;
   return result;
}

static uint32_t blend(uint32_t foreground, uint32_t background) {
    uint8_t *fg = (uint8_t *)&foreground;
    uint8_t *bg = (uint8_t *)&background;
    uint8_t result[4];
    unsigned int alpha = fg[3] + 1;
    unsigned int inv_alpha = 256 - fg[3];
    result[0] = (unsigned char)((alpha * fg[0] + inv_alpha * bg[0]) >> 8);
    result[1] = (unsigned char)((alpha * fg[1] + inv_alpha * bg[1]) >> 8);
    result[2] = (unsigned char)((alpha * fg[2] + inv_alpha * bg[2]) >> 8);
    result[3] = 0xff;

    return *(uint32_t *)&result[0];
}

void draw_image(struct image *im, struct framebuffer *fb, int start_x,
    int start_y, int width_x, int width_y, int center)
{
    size_t image_x_start, image_y_start;
    if (center) {
        image_x_start = (width_x / 2) - (im->image_width  / 2);
        image_y_start = (width_y / 2) - (im->image_height / 2);
    } else {
        image_x_start = 0;
        image_y_start = 0;
    }

    size_t image_x_length = im->image_width  > width_x ? width_x : im->image_width;
    size_t image_y_length = im->image_height > width_y ? width_y : im->image_height;

    // FIXME: This assumes RGBA big endian.
    for (size_t y = 0; y < image_y_length; y++) {
       png_byte* row = im->image_rows[y];
       for (size_t x = 0; x < image_x_length; x++) {
          uint32_t new_pixel = *((uint32_t *)&(row[x * 4]));
          uint32_t old_pixel = get_pixel(fb, image_x_start + start_x + x,
                  image_y_start + start_y + y);
          new_pixel = blend(new_pixel, old_pixel);
          new_pixel = ((new_pixel >> 16) & 0x0000FF) | ((new_pixel << 16) & 0xFF0000) | (new_pixel & 0x00FF00);
          draw_pixel(fb, image_x_start + start_x + x,
                  image_y_start + start_y + y, new_pixel);
       }
   }
}

void destroy_image(struct image *im) {
    // free(im->image_data);
    free(im);
}
