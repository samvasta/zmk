/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_meta_mod

#include <device.h>
#include <drivers/behavior.h>
#include <logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/behavior.h>
#include <zmk/meta.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int behavior_meta_mod_init(const struct device *dev) { return 0; };

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    uint32_t key = zmk_meta_mode() == 0 ? binding->param1 : binding->param2;

    LOG_DBG("position %d keycode 0x%02X", event.position, key);
    return ZMK_EVENT_RAISE(
        zmk_keycode_state_changed_from_encoded(key, true, event.timestamp));
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    uint32_t key = zmk_meta_mode() == 0 ? binding->param1 : binding->param2;
    LOG_DBG("position %d keycode 0x%02X", event.position, key);
    return ZMK_EVENT_RAISE(
        zmk_keycode_state_changed_from_encoded(key, false, event.timestamp));
}

static const struct behavior_driver_api behavior_meta_mod_driver_api = {
    .binding_pressed = on_keymap_binding_pressed, .binding_released = on_keymap_binding_released};

#define KP_INST(n)                                                                                 \
    DEVICE_DT_INST_DEFINE(n, behavior_meta_mod_init, NULL, NULL, NULL, APPLICATION,                \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_meta_mod_driver_api);

DT_INST_FOREACH_STATUS_OKAY(KP_INST)
