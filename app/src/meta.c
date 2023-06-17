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

#include <zmk/meta.h>

static uint8_t meta;

#if IS_ENABLED(CONFIG_SETTINGS)

static int meta_profiles_handle_set(const char *name, size_t len, settings_read_cb read_cb,
                                    void *cb_arg) {
    const char *next;

    LOG_DBG("Setting Meta value %s", name);

    if (settings_name_steq(name, "active_value", &next) && !next) {
        if (len != sizeof(meta)) {
            LOG_ERR("Invalid profile size (got %d expected %d)", len, sizeof(meta));
            return -EINVAL;
        }

        int err = read_cb(cb_arg, &meta, sizeof(meta));
        if (err <= 0) {
            LOG_ERR("Failed to handle reading meta value from settings (err %d)", err);
            return err;
        }

        LOG_DBG("Loaded %d value for meta", meta);
    }

    return 0;
};

struct settings_handler meta_handler = {.name = "meta", .h_set = meta_profiles_handle_set};

static void meta_save_profile_work(struct k_work *work) {
    settings_save_one("meta/active_value", &meta, sizeof(meta));
}

static struct k_work_delayable meta_save_work;
#endif

int zmk_meta_set(uint8_t value) {
    meta = value;

#if IS_ENABLED(CONFIG_SETTINGS)
    return k_work_reschedule(&meta_save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
#else
    return 0;
#endif
};

int zmk_meta_mode() { return meta; };

static int zmk_meta_init(const struct device *_arg) {
#if IS_ENABLED(CONFIG_SETTINGS)
    settings_subsys_init();

    int err = settings_register(&meta_handler);
    if (err) {
        LOG_ERR("Failed to setup the profile settings handler (err %d)", err);
        return err;
    }

    k_work_init_delayable(&meta_save_work, meta_save_profile_work);

    settings_load_subtree("meta");

#endif

    return 0;
}

SYS_INIT(zmk_meta_init, APPLICATION, CONFIG_ZMK_BLE_INIT_PRIORITY);