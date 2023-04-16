//  color.cpp: Color abstraction.
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

#include <color.h>

Color blend(Color c1, Color c2, uint8_t percentage) {
   float r = (float)(c2.red   - c1.red)   / 255;
   float g = (float)(c2.green - c1.green) / 255;
   float b = (float)(c2.blue  - c1.blue)  / 255;

   uint8_t new_r = (r * percentage) + c1.red;
   uint8_t new_g = (g * percentage) + c1.green;
   uint8_t new_b = (b * percentage) + c1.blue;

   return {.red = new_r, .green = new_g, .blue = new_b, .alpha = 0x00};
}
