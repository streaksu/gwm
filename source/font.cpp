//  font.cpp: Font interface and global registry.
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

#include <font.h>
#include <stdexcept>

static uint32_t blend(uint32_t c1, uint32_t c2, uint8_t p) {
   float r = (float)(((uint8_t)(c2 >> 16)) - ((uint8_t)(c1 >> 16))) / 255;
   float g = (float)(((uint8_t)(c2 >> 8))  - ((uint8_t)(c1 >> 8)))  / 255;
   float b = (float)(((uint8_t)(c2))       - ((uint8_t)(c1)))       / 255;

   uint8_t new_r = (r * p) + (uint8_t)(c1 >> 16);
   uint8_t new_g = (g * p) + (uint8_t)(c1 >> 8);
   uint8_t new_b = (b * p) + (uint8_t)(c1);

   return ((uint32_t)new_r << 16) | ((uint32_t)new_g <<  8) | new_b;
}

Font::Font(std::string const& path) {
   int error = FT_Init_FreeType(&(this->library));
   if (error) {
      throw new std::runtime_error("Could not init the font's inner library");
   }
   error = FT_New_Face(this->library, path.c_str(), 0, &(this->face));
   if (error == FT_Err_Unknown_File_Format) {
      throw new std::invalid_argument("The path points to a weird format");
   } else if (error) {
      throw new std::runtime_error("Could not retrieve the font's face");
   }
   error = FT_Set_Pixel_Sizes(this->face, 0, 12);
   if (error) {
      throw new std::runtime_error("Could not set the font's default size");
   }
}

Font::~Font() {
   // Freetype already makes sure we dont leak memory with its value types.
   // This is here for future cache management.
}

void Font::resize(int width, int height) {
   if (FT_Set_Pixel_Sizes(this->face, width, height)) {
      throw new std::invalid_argument("Bad dimensions");
   }
}

void Font::draw(struct framebuffer *fb, int x, int y, std::string const& str,
              uint32_t fg, uint32_t bg) {
   for (auto const& ch : str) {
      // Get the glyph.
      int glyph_index = FT_Get_Char_Index(this->face, ch);
      FT_Load_Glyph(this->face, glyph_index, FT_LOAD_DEFAULT);
      FT_Render_Glyph(this->face->glyph, FT_RENDER_MODE_NORMAL);

      // Calculate the positions on the character box.
      int bbox_ymax   = this->face->bbox.yMax / 64;
      int glyph_width = this->face->glyph->metrics.width / 64;
      int advance     = this->face->glyph->metrics.horiAdvance / 64;
      int x_off       = (advance - glyph_width) / 2;
      int y_off       = bbox_ymax - this->face->glyph->metrics.horiBearingY / 64;

      // Freetype gives us an array of arrays of bytes representing opacities.
      // From 255 (most opaque) to 0 (least opaque).
      for (int i = 0; i < (int)this->face->glyph->bitmap.rows; i++) {
         int row_offset = y + i + y_off;
         for (int j = 0; j < (int)this->face->glyph->bitmap.width; j++) {
            uint8_t p = this->face->glyph->bitmap.buffer [i * this->face->glyph->bitmap.pitch + j];
            uint32_t color = blend(fg, bg, 255 - p);
            // FIXME: Part of the next FIXME below.
            draw_pixel(fb, x + j + x_off, row_offset - (27), color);
         }
      }

      x += advance;
   }
}

static Font *regular_font;
static Font *bold_font;

void init_fonts(std::string const& regular_path, std::string const& bold_path) {
   regular_font = new Font(regular_path);
   bold_font    = new Font(bold_path);
}

void draw_str(struct framebuffer *fb, int x, int y, std::string const& str, uint32_t fg, uint32_t bg, bool is_bold) {
   // FIXME: For some reason we get with freetype the start of the font
   // a bit weird, this magic numbers (27 and 3) make it better as a bad fix.
   if (is_bold) {
      bold_font->draw(fb, x, y + 3, str, fg, bg);
   } else {
      regular_font->draw(fb, x, y, str, fg, bg);
   }
}
