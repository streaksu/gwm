//  main.cpp: Entrypoint of the project.
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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <window.h>
#include <fb.h>
#include <taskbar.h>
#include <cursor.h>
#include <font.h>
#include <widgets/widget.h>
#include <sys/ironclad_devices.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <config.h>

#define BACKGROUND_COLOR 0xaaaaaa

static struct framebuffer *main_fb;
static struct taskbar     *taskbar;
static struct cursor      *cursor;
static struct window      *win_array[20];

struct window *add_window(const char *name) {
    for (int i = 0; i < 20; i++) {
        if (win_array[i] == NULL) {
            struct window *intermediate = win_array[0];
            win_array[0] = create_window(name);
            if (i != 0) {
                win_array[i] = intermediate;
            }
            return win_array[0];
        }
    }

   return NULL;
}

static void focus_and_bring_to_the_top(int source) {
    focus_window(win_array[source]);
    struct window *intermediate = win_array[0];
    win_array[0] = win_array[source];
    win_array[source] = intermediate;
}

static void refresh() {
    // Clear the background.
    draw_background(main_fb);

    // Go thru the windows, check if we are clicking any, move and draw.
    // Window at index 0 is always the focused one.
    for (int i = 19; i >= 0; i--) {
        if (win_array[i] != NULL) {
            if (i != 0) {
                unfocus_window(win_array[i]);
            }
            draw_window(win_array[i], main_fb);
        }
    }

    // Write the taskbar and cursor.
    draw_taskbar(taskbar, main_fb);
    draw_cursor(cursor, main_fb);

    // Finally write.
    refresh_to_backend(main_fb);
}

int main(int argc, char *argv[]) {
   // Handle cmdline.
   char c;
   while ((c = getopt (argc, argv, "hv")) != -1) {
      switch (c) {
         case 'h':
            puts("Usage: gwm [options]");
            puts("");
            puts("Options:");
            puts("-h Print this help message");
            puts("-v Print version information");
            return 0;
         case 'v':
            puts("gwm " PACKAGE_VERSION);
            puts("Copyright (C) 2023 streaksu.");
            puts("Licensed under the GNU GPL version 3 or later.");
            puts("This is free software: you are free to change and redistribute it.");
            puts("There is NO WARRANTY, to the extent permitted by law.");
            return 0;
         default:
            fprintf(stderr, "gwm: Unknown option '%c'\n", optopt);
            return 1;
      }
   }

    // Open the framebuffer.
    int fb = open(FRAMEBUFFER_DEVICE_PATH, O_RDWR);
    if (fb == -1) {
        perror("Could not open the framebuffer");
        exit(1);
    }

    // Open the mouse.
    int ps = open(MOUSE_DEVICE_PATH, O_RDONLY);
    if (ps == -1) {
        perror("Could not open the mouse");
        exit(1);
    }

    // Set mouse modes.
    ioctl(ps, PS2MOUSE_2_1_SCALING,     0);
    ioctl(ps, PS2MOUSE_SET_RES,         3);
    ioctl(ps, PS2MOUSE_SET_SAMPLE_RATE, 100);

    // Init fonts.
    try {
      init_fonts(REGULAR_FONT_PATH, BOLD_FONT_PATH);
    } catch (...) {
        perror("Could not open fonts");
        exit(1);
    }

    // Create a framebuffer from that one.
    main_fb = create_framebuffer_from_fd(fb);
    if (main_fb == NULL) {
        perror("Could not create main framebuffer");
        exit(1);
    }

    // Initialize desktop widgets and create the welcome window.
    taskbar = create_taskbar(main_fb);
    if (taskbar == NULL) {
        perror("Could not create taskbar");
        exit(1);
    }

    cursor = create_cursor();
    if (cursor == NULL) {
        perror("Could not create cursor");
        exit(1);
    }

    struct window *win = add_window("Welcome!");
    add_child(win, create_image("/etc/gwm-gloire.png", 1));
    add_child(win, create_textbox(
        "Gloire\n"
        "An Ironclad distribution using mlibc and\n"
        "several GNU tools. Made with love <3\n"
        "\n"
        "Copyright (C) 2023 streaksu\n"
        "Have a nice time around!", 1
    ));

    // Initial refresh and loop waiting for mouse.
    for (;;) {
        refresh();

        struct ironclad_mouse_data data;
        read(ps, &data, sizeof(struct ironclad_mouse_data));

        size_t old_cursor_x = cursor->x_position;
        size_t old_cursor_y = cursor->y_position;
        update_cursor(cursor, main_fb, data.x_variation, data.y_variation);
        if (!data.is_left && !data.is_right) {
            continue;
        }

        for (int i = 0; i < 20; i++) {
            if (win_array[i] == NULL) {
                continue;
            }
            switch (click_window(win_array[i], old_cursor_x, old_cursor_y)) {
                case WINDOW_PLEASE_CLOSE:
                    destroy_window(win_array[i]);
                    win_array[i] = NULL;
                    goto DONE;
                case WINDOW_BAR_CLICK:
                    focus_and_bring_to_the_top(i);
                    move_window(win_array[0], data.x_variation,
                        data.y_variation, 0, 0, main_fb->pixel_width,
                        main_fb->pixel_height
                    );
                    goto DONE;
                case WINDOW_CONTENT_CLICK:
                    focus_and_bring_to_the_top(i);
                    goto DONE;
                case WINDOW_NOT_TOUCHED:
                    unfocus_window(win_array[i]);
                    break;
            }
        }
DONE:;
    }
}
