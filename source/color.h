//  color.h: Color abstraction.
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

#include <cstdint>

// POD to represent a color regardless of inner representation, be it
// RGBA, ARGB, RAGB, or RRASATATSAFASDFDe.
struct Color {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
   uint8_t alpha; // 0 - most opaque, 255 - least opaque.
};

// Blend 2 colors with a percentage of the first vs the other (0 <-> 255).
Color blend(Color c1, Color c2, uint8_t percentage);
