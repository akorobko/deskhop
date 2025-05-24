/*
 * This file is part of DeskHop (https://github.com/hrvach/deskhop).
 * Copyright (c) 2025 Hrvoje Cavrak
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * See the file LICENSE for the full license text.
 */
#include "main.h"
#include "c_cpp_compat.h"

/* Default configuration */
const config_t default_config = {
    /* magic_header */ 0xB00B1E5,
    /* version */ CURRENT_CONFIG_VERSION,
    /* force_mouse_boot_mode */ false,
    /* force_kbd_boot_protocol */ false,
    /* kbd_led_as_indicator */ KBD_LED_AS_INDICATOR,
    /* hotkey_toggle */ 1, // Default to enabled
    /* enable_acceleration */ 1, // Default to enabled
    /* enforce_ports */ ENFORCE_PORTS,
    /* jump_threshold */ 5, // Default value
    /* output */ {
        {
            /* number */ OUTPUT_A,
            /* speed_x */ MOUSE_SPEED_A_FACTOR_X,
            /* speed_y */ MOUSE_SPEED_A_FACTOR_Y,
            /* border */ {
                /* top */ 0,
                /* bottom */ MAX_SCREEN_COORD,
            },
            /* screen_count */ 1,
            /* screen_index */ 1,
            /* os */ OUTPUT_A_OS,
            /* pos */ RIGHT,
            /* screensaver */ {
                /* mode */ SCREENSAVER_A_MODE,
                /* only_if_inactive */ SCREENSAVER_A_ONLY_IF_INACTIVE,
                /* idle_time_us */ (uint64_t)SCREENSAVER_A_IDLE_TIME_SEC * 1000000,
                /* max_time_us */ (uint64_t)SCREENSAVER_A_MAX_TIME_SEC * 1000000,
            }
        },
        {
            /* number */ OUTPUT_B,
            /* speed_x */ MOUSE_SPEED_B_FACTOR_X,
            /* speed_y */ MOUSE_SPEED_B_FACTOR_Y,
            /* border */ {
                /* top */ 0,
                /* bottom */ MAX_SCREEN_COORD,
            },
            /* screen_count */ 1,
            /* screen_index */ 1,
            /* os */ OUTPUT_B_OS,
            /* pos */ LEFT,
            /* screensaver */ {
                /* mode */ SCREENSAVER_B_MODE,
                /* only_if_inactive */ SCREENSAVER_B_ONLY_IF_INACTIVE,
                /* idle_time_us */ (uint64_t)SCREENSAVER_B_IDLE_TIME_SEC * 1000000,
                /* max_time_us */ (uint64_t)SCREENSAVER_B_MAX_TIME_SEC * 1000000,
            }
        }
    },
    /* _reserved */ 0,
    /* checksum */ 0
};
