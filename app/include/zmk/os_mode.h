/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

int zmk_os_mode_a();
int zmk_os_mode_b();
int zmk_os_mode_toggle();
int zmk_os_mode_select(uint8_t index);

int zmk_active_os_mode();
