//  font.h: Font interface and global registry.
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

#include <string>
#include <stddef.h>
#include <fb.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>

class Font {
   FT_Library library;
   FT_Face    face;

public:
   // Path must point to a TTF font.
   // In failure, std::invalid_argument or std::runtime_error will be thrown.
   Font(std::string const& path);

   // Standard destructor.
   ~Font() noexcept;

   // Resize the font in pixels. 0 for width indicates "the same as height".
   // In failure, std::invalid_argument will be thrown.
   void resize(int width, int height);

   // Draw a string using the font in the passed coordinates.
   // This function does not fail.
   void draw(struct framebuffer *fb, int x, int y, std::string const& str,
             uint32_t fg, uint32_t bg);
};

// The following wrapper is provided for comfort. It registers some fonts and
// allows use by dispatching for the specific font with some missing
// capabilities.

// Initialize local fonts for use, both paths for a regular and bold font must
// be TTF fonts. In failure, the function will throw.
void init_fonts(std::string const& regular_path, std::string const& bold_path);

// Draw a string using one of the previously assigned fonts.
void draw_str(struct framebuffer *fb, int x, int y, std::string const& str, uint32_t fg, uint32_t bg, bool is_bold);
