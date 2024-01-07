/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_multi_function_set

#include <zephyr/device.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <drivers/behavior.h>
#include <dt-bindings/zmk/multi_function.h>
#include <zmk/behavior.h>

#include <zmk/multi_function.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
    case MULTI_FN_SET_CMD:
        return zmk_active_mode_set(binding->param2);
    case MULTI_FN_NXT_CMD:
        zmk_active_mode_set(zmk_active_mode() + 1);
        return ZMK_BEHAVIOR_OPAQUE;
    case MULTI_FN_PRV_CMD:
        if (zmk_active_mode() > 0) {
            zmk_active_mode_set(zmk_active_mode() - 1);
        }
        return ZMK_BEHAVIOR_OPAQUE;
    default:
        LOG_ERR("Unknown Multi Function Set command: %d", binding->param1);
    }

    return -ENOTSUP;
}

static int behavior_multi_function_set_init(const struct device *dev) { return 0; };

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_multi_function_set_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

BEHAVIOR_DT_INST_DEFINE(0, behavior_multi_function_set_init, NULL, NULL, NULL, APPLICATION,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_multi_function_set_driver_api);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
