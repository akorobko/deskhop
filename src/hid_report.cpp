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
#include "hid_report.h"
#include "main.h"
#include "c_cpp_compat.h"

/* Given a value struct with size and offset in bits, find and return a value from the HID report */
int32_t get_report_value(uint8_t *report, int len, report_val_t *val) {
    /* Calculate the bit offset within the byte */
    uint16_t offset_in_bits = val->offset % 8;

    /* Calculate the remaining bits in the first byte */
    uint16_t remaining_bits = 8 - offset_in_bits;

    /* Calculate the byte offset in the array */
    uint16_t byte_offset = val->offset >> 3;

    if (byte_offset >= len)
        return 0;

    /* Create a mask for the specified number of bits */
    uint32_t mask = (1u << val->size) - 1;

    /* If the entire value is contained within a single byte */
    if (val->size <= remaining_bits) {
        /* Extract the bits from the report */
        uint32_t v = (report[byte_offset] >> offset_in_bits) & mask;
        return v;
    }

    /* The value spans multiple bytes */
    uint32_t result = 0;
    uint32_t total_bits_processed = 0;

    /* Process the first byte */
    result |= (report[byte_offset] >> offset_in_bits) & ((1u << remaining_bits) - 1);
    total_bits_processed += remaining_bits;

    /* Process the middle bytes */
    while (total_bits_processed + 8 <= val->size) {
        byte_offset++;
        if (byte_offset >= len) return result;

        result |= (uint32_t)report[byte_offset] << total_bits_processed;
        total_bits_processed += 8;
    }

    /* Process the last byte if there are any bits left */
    if (total_bits_processed < val->size) {
        byte_offset++;
        if (byte_offset >= len) return result;

        uint32_t remaining = val->size - total_bits_processed;
        uint32_t last_byte_bits = report[byte_offset] & ((1u << remaining) - 1);
        result |= last_byte_bits << total_bits_processed;
    }

    return result;
}

void filter_bit_values(report_val_t *src, uint8_t *report, int len) {
    /* If the report is not in variable mode, we need to handle it differently */
    if (src->data_type != VARIABLE) {
        for (int i = 0; i < src->usage_max - src->usage_min + 1; i++) {
            int32_t usage_n = get_report_value(report, len, src);
            if (i < usage_n)
                TU_LOG1("Usage %d\r\n", src->usage_min + i);
        }
        return;
    }

    /* For variable data */
    uint32_t value = get_report_value(report, len, src);

    /* Iterate through all the bits */
    for (uint32_t i = 0; i < src->size; i++) {
        if (value & (1 << i)) {
            TU_LOG1("Usage %d\r\n", src->usage_min + i);
        }
    }
}

// Helper function to extract value directly from descriptor
static void _store(report_val_t *src, report_val_t *dst, hid_interface_t *iface) {
    *dst = *src;
    iface->uses_report_id |= (src->report_id > 0);
}

// Handle button bit for the mouse button array
static void handle_button_bit(report_val_t *src, report_val_t *dst, hid_interface_t *iface) {
    _store(src, dst, iface);
}

// Extract keyboard data values
static void handle_keyboard_descriptor_values(report_val_t *src, report_val_t *dst, hid_interface_t *iface) {
    if (src->usage_page == HID_USAGE_PAGE_KEYBOARD) {
        if (src->data_type == VARIABLE) {
            _store(src, &iface->keyboard.modifier, iface);
            iface->keyboard.is_found = true;
        }
        else if (src->data_type == ARRAY) {
            // Calculate key press count
            _store(src, &iface->keyboard.nkro, iface);
            iface->keyboard.is_found = true;
            iface->keyboard.is_nkro = false;
        }

        /* Remember the report ID for sending responses back */
        iface->keyboard.report_id = src->report_id;
    }
}

// Handle consumer control values
static void handle_consumer_control_values(report_val_t *src, report_val_t *dst, hid_interface_t *iface) {
    if (src->usage_page == HID_USAGE_PAGE_CONSUMER) {
        _store(src, dst, iface);
        iface->consumer.is_variable = (src->data_type == VARIABLE);
        iface->consumer.is_array = (src->data_type == ARRAY);
        iface->consumer.report_id = src->report_id;
    }
}

/* Choose what values from the report to extract, based on the HID usage info */
void extract_data(hid_interface_t *iface, report_val_t *val) {
    /* Create an array of complex HID value mappings */
    const usage_map_t map[] = {
        /* Map mouse buttons */
        usage_map_t{
            /* global_usage */ HID_USAGE_DESKTOP_MOUSE,
            /* usage_page */ HID_USAGE_PAGE_BUTTON,
            /* usage */ 0,
            /* id */ &iface->mouse.report_id,
            /* dst */ &iface->mouse.buttons,
            /* handler */ handle_button_bit,
            /* receiver */ process_mouse_report
        },

        /* Map mouse X axis */
        usage_map_t{
            /* global_usage */ HID_USAGE_DESKTOP_MOUSE,
            /* usage_page */ HID_USAGE_PAGE_DESKTOP,
            /* usage */ HID_USAGE_DESKTOP_X,
            /* id */ &iface->mouse.report_id,
            /* dst */ &iface->mouse.move_x,
            /* handler */ _store,
            /* receiver */ process_mouse_report
        },

        /* Map mouse Y axis */
        usage_map_t{
            /* global_usage */ HID_USAGE_DESKTOP_MOUSE,
            /* usage_page */ HID_USAGE_PAGE_DESKTOP,
            /* usage */ HID_USAGE_DESKTOP_Y,
            /* id */ &iface->mouse.report_id,
            /* dst */ &iface->mouse.move_y,
            /* handler */ _store,
            /* receiver */ process_mouse_report
        },

        /* Map mouse scroll wheel */
        usage_map_t{
            /* global_usage */ HID_USAGE_DESKTOP_MOUSE,
            /* usage_page */ HID_USAGE_PAGE_DESKTOP,
            /* usage */ HID_USAGE_DESKTOP_WHEEL,
            /* id */ &iface->mouse.report_id,
            /* dst */ &iface->mouse.wheel,
            /* handler */ _store,
            /* receiver */ process_mouse_report
        },

        /* Map mouse horizontal scroll (pan) */
        usage_map_t{
            /* global_usage */ HID_USAGE_DESKTOP_MOUSE,
            /* usage_page */ HID_USAGE_PAGE_CONSUMER,
            /* usage */ HID_USAGE_CONSUMER_AC_PAN,
            /* id */ &iface->mouse.report_id,
            /* dst */ &iface->mouse.pan,
            /* handler */ _store,
            /* receiver */ process_mouse_report
        },

        /* Map keyboard */
        usage_map_t{
            /* global_usage */ HID_USAGE_DESKTOP_KEYBOARD,
            /* usage_page */ HID_USAGE_PAGE_KEYBOARD,
            /* usage */ 0,
            /* id */ &iface->keyboard.report_id,
            /* dst */ nullptr,
            /* handler */ handle_keyboard_descriptor_values,
            /* receiver */ process_keyboard_report
        },

        /* Map consumer control */
        usage_map_t{
            /* global_usage */ HID_USAGE_CONSUMER_CONTROL,
            /* usage_page */ HID_USAGE_PAGE_CONSUMER,
            /* usage */ 0,
            /* id */ &iface->consumer.report_id,
            /* dst */ &iface->consumer.val,
            /* handler */ handle_consumer_control_values,
            /* receiver */ process_consumer_report
        },

        /* Map system control */
        usage_map_t{
            /* global_usage */ HID_USAGE_DESKTOP_SYSTEM_CONTROL,
            /* usage_page */ HID_USAGE_PAGE_DESKTOP,
            /* usage */ 0,
            /* id */ &iface->system.report_id,
            /* dst */ &iface->system.val,
            /* handler */ _store,
            /* receiver */ process_system_report
        },
    };

    /* We extracted all we could find in the descriptor to report_values, now go through them and
       match them up with the values in the table above, then store those values for later reference */

    for (const usage_map_t *hay = map; hay != &map[ARRAY_SIZE(map)]; hay++) {
        /* ---> If any condition is not defined, we consider it as matched <--- */
        bool global_usages_match = (val->global_usage == hay->global_usage) || (hay->global_usage == 0);
        bool usages_match        = (val->usage == hay->usage) || (hay->usage == 0);
        bool usage_pages_match   = (val->usage_page == hay->usage_page) || (hay->usage_page == 0);

        if (val->item_type == CONSTANT)
            continue;

        /* If we have a complete match */
        if (global_usages_match && usages_match && usage_pages_match) {
            /* Invoke the data handler for that field (store it directly or do some manipulations) */
            hay->handler(val, hay->dst, iface);

            /* Mark the corresponding report ID as a identifier for this handler */
            iface->report_handler[val->report_id] = hay->receiver;

            break;
        }
    }
}

/* Given a receiver function and report ID, verify this is a valid HID report handler */
process_report_f get_report_handler(hid_interface_t *iface, uint8_t report_id) {
    for (int i = 0; i < MAX_REPORTS; i++) {
        if (iface->report_handler[i] != NULL && (i == report_id || !iface->uses_report_id || report_id == 0))
            return iface->report_handler[i];
    }

    // This might happen when the report doesn't use a report ID, e.g. has only one report
    return NULL;
}

/* Extract a variable from a bit field report */
int32_t extract_bit_variable(report_val_t *val, uint8_t *report, int len, uint8_t *report_id) {
    if (report == NULL || val == NULL)
        return 0;

    /* If a report ID is defined, it has to be the first byte of the report */
    if (val->report_id > 0 && report[0] != val->report_id)
        return 0;

    /* If the report uses an ID, store it for later use */
    if (report_id)
        *report_id = val->report_id;

    // Adjust offset for report ID (used variable must match warning fix)
    report_val_t adjusted_val = *val;
    if (val->report_id > 0) {
        adjusted_val.offset += 8;
    }

    /* Do the actual extraction */
    return get_report_value(report, len, &adjusted_val);
}
