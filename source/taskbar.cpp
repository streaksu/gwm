//  taskbar.cpp: Taskbar creation and management.
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

#include <taskbar.h>
#include <stdlib.h>
#include <font.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <config.h>

#define TASKBAR_HEIGHT 20
#define TASKBAR_COLOR  0x444444
#define TASKBAR_TEXT   0xFFFFFF

static time_t last_time = 0;

static void draw_time(struct framebuffer *fb) {
    char time_str[80];
    struct tm *tm = localtime(&last_time);
    size_t time_str_len = strftime(time_str, 80, "%a %b %d, %H:%M", tm);
    size_t start_time = fb->pixel_width - ((time_str_len + 1) * 8);
    draw_string(fb, start_time, 0, time_str, time_str_len, TASKBAR_TEXT, TASKBAR_COLOR);
}

static void *time_update_thread(void *fb_arg) {
    struct framebuffer *fb = static_cast<struct framebuffer *>(fb_arg);

    for (;;) {
        time_t new_time = time(NULL);
        if (new_time > last_time) {
            last_time = new_time;
            draw_time(fb);
            refresh_to_backend(fb);
        }
        sched_yield();
    }
}

struct taskbar *create_taskbar(struct framebuffer *fb) {
    // Assign the current time.
    last_time = time(NULL);

    // Create the taskbar.
    struct taskbar *ret = new struct taskbar;
    ret->logo = init_image(APPLICATIONS_ICON_PATH);
    pthread_create(&ret->time_update_id, NULL, time_update_thread, fb);
    return ret;
}

void draw_taskbar(struct taskbar *bar, struct framebuffer *fb) {
    draw_rectangle(fb, 0, 0, fb->pixel_width, TASKBAR_HEIGHT, TASKBAR_COLOR);
    draw_image(bar->logo, fb, 8, 0, 100000, 10000, 0);
    draw_time(fb);
}
