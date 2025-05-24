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
const config_t default_config = []() {
    config_t cfg = {};  // zero initialize

    // Set magic header and version
    cfg.magic_header = 0xB00B1E5;
    cfg.version = CURRENT_CONFIG_VERSION;
    
    // Boot mode options
    cfg.force_mouse_boot_mode = false;
    cfg.force_kbd_boot_protocol = false;
    
    // Features
    cfg.kbd_led_as_indicator = KBD_LED_AS_INDICATOR;
    cfg.hotkey_toggle = 1; // Default to enabled
    cfg.enable_acceleration = 1; // Default to enabled
    cfg.enforce_ports = ENFORCE_PORTS;
    cfg.jump_threshold = 5; // Default value
    
    // Configure Output A
    cfg.output[OUTPUT_A].number = OUTPUT_A;
    cfg.output[OUTPUT_A].speed_x = MOUSE_SPEED_A_FACTOR_X;
    cfg.output[OUTPUT_A].speed_y = MOUSE_SPEED_A_FACTOR_Y;
    cfg.output[OUTPUT_A].border.top = 0;
    cfg.output[OUTPUT_A].border.bottom = MAX_SCREEN_COORD;
    cfg.output[OUTPUT_A].screen_count = 1;
    cfg.output[OUTPUT_A].screen_index = 1;
    cfg.output[OUTPUT_A].os = OUTPUT_A_OS;
    cfg.output[OUTPUT_A].pos = RIGHT;
    cfg.output[OUTPUT_A].screensaver.mode = SCREENSAVER_A_MODE;
    cfg.output[OUTPUT_A].screensaver.only_if_inactive = SCREENSAVER_A_ONLY_IF_INACTIVE;
    cfg.output[OUTPUT_A].screensaver.idle_time_us = (uint64_t)SCREENSAVER_A_IDLE_TIME_SEC * 1000000;
    cfg.output[OUTPUT_A].screensaver.max_time_us = (uint64_t)SCREENSAVER_A_MAX_TIME_SEC * 1000000;
    
    // Configure Output B
    cfg.output[OUTPUT_B].number = OUTPUT_B;
    cfg.output[OUTPUT_B].speed_x = MOUSE_SPEED_B_FACTOR_X;
    cfg.output[OUTPUT_B].speed_y = MOUSE_SPEED_B_FACTOR_Y;
    cfg.output[OUTPUT_B].border.top = 0;
    cfg.output[OUTPUT_B].border.bottom = MAX_SCREEN_COORD;
    cfg.output[OUTPUT_B].screen_count = 1;
    cfg.output[OUTPUT_B].screen_index = 1;
    cfg.output[OUTPUT_B].os = OUTPUT_B_OS;
    cfg.output[OUTPUT_B].pos = LEFT;
    cfg.output[OUTPUT_B].screensaver.mode = SCREENSAVER_B_MODE;
    cfg.output[OUTPUT_B].screensaver.only_if_inactive = SCREENSAVER_B_ONLY_IF_INACTIVE;
    cfg.output[OUTPUT_B].screensaver.idle_time_us = (uint64_t)SCREENSAVER_B_IDLE_TIME_SEC * 1000000;
    cfg.output[OUTPUT_B].screensaver.max_time_us = (uint64_t)SCREENSAVER_B_MAX_TIME_SEC * 1000000;
    
    // Final settings
    cfg._reserved = 0;
    cfg.checksum = 0;
    
    return cfg;
}();
