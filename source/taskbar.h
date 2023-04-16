//  taskbar.h: Taskbar creation and management.
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

#include <stdbool.h>
#include <fb.h>
#include <pthread.h>
#include <widgets/image.h>

struct taskbar {
    struct image *logo;
    pthread_t time_update_id;
};

struct taskbar *create_taskbar(struct framebuffer *fb);
void draw_taskbar(struct taskbar *bar, struct framebuffer *fb);
