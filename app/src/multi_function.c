/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/init.h>

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#if IS_ENABLED(CONFIG_SETTINGS)

#include <zephyr/settings/settings.h>

#endif

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/multi_function.h>

static uint8_t active_mode;

#if IS_ENABLED(CONFIG_SETTINGS)

static int multi_function_profiles_handle_set(const char *name, size_t len,
                                              settings_read_cb read_cb, void *cb_arg) {
    const char *next;

    LOG_DBG("Setting Meta value %s", name);

    if (settings_name_steq(name, "active_value", &next) && !next) {
        if (len != sizeof(active_mode)) {
            LOG_ERR("Invalid profile size (got %d expected %d)", len, sizeof(active_mode));
            return -EINVAL;
        }

        int err = read_cb(cb_arg, &active_mode, sizeof(active_mode));
        if (err <= 0) {
            LOG_ERR("Failed to handle reading active_mode value from settings (err %d)", err);
            return err;
        }

        LOG_DBG("Loaded %d value for active_mode", active_mode);
    }

    return 0;
};

struct settings_handler multi_function_handler = {.name = "multi_function",
                                                  .h_set = multi_function_profiles_handle_set};

static void multi_function_save_profile_work(struct k_work *work) {
    settings_save_one("multi_function/active_value", &active_mode, sizeof(active_mode));
}

static struct k_work_delayable multi_function_save_work;
#endif

int zmk_active_mode_set(uint8_t value) {
    active_mode = value;

#if IS_ENABLED(CONFIG_SETTINGS)
    return k_work_reschedule(&multi_function_save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
#else
    return 0;
#endif
};

int zmk_active_mode() { return active_mode; };

static int zmk_multi_function_init(const struct device *_arg) {
#if IS_ENABLED(CONFIG_SETTINGS)
    settings_subsys_init();

    int err = settings_register(&multi_function_handler);
    if (err) {
        LOG_ERR("Failed to setup the profile settings handler (err %d)", err);
        return err;
    }

    k_work_init_delayable(&multi_function_save_work, multi_function_save_profile_work);

    settings_load_subtree("multi_function");

#endif

    return 0;
}

SYS_INIT(zmk_multi_function_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);