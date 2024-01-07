/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <zephyr/types.h>

int zmk_active_mode();
int zmk_active_mode_set(uint8_t mode);