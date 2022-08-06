/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr.h>
#include <zmk/event_manager.h>
#include <device.h>

#include <zmk/ble/profile.h>

struct zmk_active_os_mode_changed {
    uint8_t current_mode;
};

ZMK_EVENT_DECLARE(zmk_active_os_mode_changed);
