/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_meta_mod_set

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>

#include <zmk/matrix.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/modifiers_state_changed.h>
#include <zmk/hid.h>
#include <zmk/meta.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int on_meta_mod_set_binding_pressed(struct zmk_behavior_binding *binding,
                                           struct zmk_behavior_binding_event event) {

    zmk_meta_set(binding->param1);

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_meta_mod_set_binding_released(struct zmk_behavior_binding *binding,
                                            struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_meta_mod_set_driver_api = {
    .binding_pressed = on_meta_mod_set_binding_pressed,
    .binding_released = on_meta_mod_set_binding_released,
};

static int behavior_meta_mod_set_init(const struct device *dev) { return 0; }

#define META_MOD_SET_INST(n)                                                                       \
    DEVICE_DT_INST_DEFINE(n, behavior_meta_mod_set_init, NULL, NULL, NULL, APPLICATION,            \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_meta_mod_set_driver_api);

DT_INST_FOREACH_STATUS_OKAY(META_MOD_SET_INST)

#endif
