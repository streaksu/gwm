//  fb.cpp: Framebuffer management.
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

#include <fb.h>
#include <font.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

struct framebuffer *create_framebuffer_from_fd(int fd) {
    // Fetch fb info.
    struct fb_var_screeninfo var_info;
    struct fb_fix_screeninfo fix_info;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var_info) == -1) {
        return NULL;
    }
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
        return NULL;
    }

    // Get dimensions.
    size_t pixel_size  = fix_info.smem_len / sizeof(uint32_t);
    size_t linear_size = pixel_size * sizeof(uint32_t);

    // Mmap the framebuffer into our mem_window.
    size_t aligned_size = (linear_size + 0x1000 - 1) & ~(0x1000 - 1);
    uint32_t *mem_window = static_cast<uint32_t *>(mmap(NULL, aligned_size, PROT_READ | PROT_WRITE,
       MAP_SHARED, fd, 0));
    if (mem_window == NULL) {
        return NULL;
    }

    // Allocate the memory representation and get the current one for
    // background purposes.
    uint32_t *antibuffer  = new uint32_t[linear_size];
    uint32_t *frontbuffer = new uint32_t[linear_size];
    uint32_t *background  = new uint32_t[linear_size];
    if (antibuffer == NULL || frontbuffer == NULL) {
        return NULL;
    }

    // Load a background and put it in the buffers.
    for (size_t i = 0; i < pixel_size; i++) {
        background[i] = i % 7 == 0 ? 0x605885 : 0xC0C0C0;
    }
    memcpy(mem_window,  background, linear_size);
    memcpy(antibuffer,  background, linear_size);
    memcpy(frontbuffer, background, linear_size);

    struct image *img_background = init_image("/etc/gwm-background.png");

    // Allocate the final object and return it.
    struct framebuffer *ret = new struct framebuffer;
    if (ret == NULL) {
        return NULL;
    }
    ret->backing_fd    = fd;
    ret->background    = img_background;
    ret->antibuffer    = antibuffer;
    ret->frontbuffer   = frontbuffer;
    ret->memory_window = mem_window;
    ret->pixel_width   = var_info.xres;
    ret->pixel_height  = var_info.yres;
    ret->pitch         = fix_info.smem_len / var_info.yres;
    ret->pixel_size    = pixel_size;
    ret->linear_size   = linear_size;
    return ret;
}

void refresh_to_backend(struct framebuffer *fb) {
    // Compare changes with the front buffer and write to the window.
    for (uint64_t i = 0; i < fb->pixel_size; i++) {
        if (fb->frontbuffer[i] != fb->antibuffer[i]) {
            fb->frontbuffer[i]   = fb->antibuffer[i];
            fb->memory_window[i] = fb->antibuffer[i];
        }
    }
}

void draw_background(struct framebuffer *fb) {
    draw_image(fb->background, fb, 0, 0, fb->pixel_width, fb->pixel_height, 0);
}

void draw_pixel(struct framebuffer *fb, int x, int y, uint32_t color) {
    if ((size_t)x >= fb->pixel_width || (size_t)y >= fb->pixel_height || x < 0 || y < 0) {
        return;
    }
    fb->antibuffer[x + (fb->pitch / sizeof (uint32_t)) * y] = color;
}

uint32_t get_pixel(struct framebuffer *fb, int x, int y) {
    if ((size_t)x >= fb->pixel_width || (size_t)y >= fb->pixel_height || x < 0 || y < 0) {
        return 0;
    }
    return fb->antibuffer[x + (fb->pitch / sizeof (uint32_t)) * y];
}

void draw_rectangle(struct framebuffer *fb, int x, int y, int x_end, int y_end, uint32_t color) {
    for (int i = x; i < x_end; i++) {
        for (int j = y; j < y_end; j++) {
            draw_pixel(fb, i, j, color);
        }
    }
}

void draw_string(struct framebuffer *fb, int x, int y, const char *str, size_t char_count, uint32_t fg, uint32_t bg) {
    std::string str2(str, char_count);
    draw_str(fb, x, y, str2, fg, bg, 0);
}
