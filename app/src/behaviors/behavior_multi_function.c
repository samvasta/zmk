/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_multi_function

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk/matrix.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/hid.h>
#include <zmk/multi_function.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define ZMK_BHV_TAP_DANCE_MAX_HELD 10

#define ZMK_BHV_TAP_DANCE_POSITION_FREE UINT32_MAX

struct behavior_multi_function_config {
    size_t behavior_count;
    struct zmk_behavior_binding *behaviors;
};

static int on_multi_function_binding_pressed(struct zmk_behavior_binding *binding,
                                             struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_multi_function_config *cfg = dev->config;

    int index = zmk_active_mode() < 0                      ? 0
                : zmk_active_mode() >= cfg->behavior_count ? cfg->behavior_count - 1
                                                           : zmk_active_mode();

    struct zmk_behavior_binding mode_binding = cfg->behaviors[index];

    return behavior_keymap_binding_pressed(&mode_binding, event);
}

static int on_multi_function_binding_released(struct zmk_behavior_binding *binding,
                                              struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_multi_function_config *cfg = dev->config;

    int index = zmk_active_mode() < 0                      ? 0
                : zmk_active_mode() >= cfg->behavior_count ? cfg->behavior_count - 1
                                                           : zmk_active_mode();

    struct zmk_behavior_binding mode_binding = cfg->behaviors[index];

    return behavior_keymap_binding_released(&mode_binding, event);
}

static const struct behavior_driver_api behavior_multi_function_driver_api = {
    .binding_pressed = on_multi_function_binding_pressed,
    .binding_released = on_multi_function_binding_released,
};

static int behavior_multi_function_init(const struct device *dev) { return 0; }

#define _TRANSFORM_ENTRY(idx, node) ZMK_KEYMAP_EXTRACT_BINDING(idx, node)

#define TRANSFORMED_BINDINGS(node)                                                                 \
    { LISTIFY(DT_INST_PROP_LEN(node, bindings), _TRANSFORM_ENTRY, (, ), DT_DRV_INST(node)) }

#define KP_INST(n)                                                                                 \
    static struct zmk_behavior_binding                                                             \
        behavior_multi_function_config_##n##_bindings[DT_INST_PROP_LEN(n, bindings)] =             \
            TRANSFORMED_BINDINGS(n);                                                               \
    static struct behavior_multi_function_config behavior_multi_function_config_##n = {            \
        .behaviors = behavior_multi_function_config_##n##_bindings,                                \
        .behavior_count = DT_INST_PROP_LEN(n, bindings)};                                          \
    BEHAVIOR_DT_INST_DEFINE(                                                                       \
        n, behavior_multi_function_init, NULL, NULL, &behavior_multi_function_config_##n,          \
        APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_multi_function_driver_api);

DT_INST_FOREACH_STATUS_OKAY(KP_INST)

#endif