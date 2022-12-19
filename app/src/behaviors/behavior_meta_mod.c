/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_meta_mod

#include <device.h>
#include <drivers/behavior.h>
#include <logging/log.h>
#include <zmk/behavior.h>

#include <zmk/matrix.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/modifiers_state_changed.h>
#include <zmk/hid.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct behavior_meta_mod_config {
    struct zmk_behavior_binding normal_binding;
    struct zmk_behavior_binding morph_binding;
    zmk_mod_flags_t mods;
};

struct behavior_meta_mod_data {
    struct zmk_behavior_binding *pressed_binding;
};

static int on_meta_mod_binding_pressed(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    const struct device *dev = device_get_binding(binding->behavior_dev);
    const struct behavior_meta_mod_config *cfg = dev->config;
    struct behavior_meta_mod_data *data = dev->data;

    if (data->pressed_binding != NULL) {
        LOG_ERR("Can't press the same mod-morph twice");
        return -ENOTSUP;
    }

    if (zmk_hid_get_explicit_mods() & cfg->mods) {
        data->pressed_binding = (struct zmk_behavior_binding *)&cfg->morph_binding;
    } else {
        data->pressed_binding = (struct zmk_behavior_binding *)&cfg->normal_binding;
    }
    return behavior_keymap_binding_pressed(data->pressed_binding, event);
}

static int on_meta_mod_binding_released(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    const struct device *dev = device_get_binding(binding->behavior_dev);
    struct behavior_meta_mod_data *data = dev->data;

    if (data->pressed_binding == NULL) {
        LOG_ERR("Mod-morph already released");
        return -ENOTSUP;
    }

    struct zmk_behavior_binding *pressed_binding = data->pressed_binding;
    data->pressed_binding = NULL;
    return behavior_keymap_binding_released(pressed_binding, event);
}

static const struct behavior_driver_api behavior_meta_mod_driver_api = {
    .binding_pressed = on_meta_mod_binding_pressed,
    .binding_released = on_meta_mod_binding_released,
};

static int behavior_meta_mod_init(const struct device *dev) { return 0; }

#define _TRANSFORM_ENTRY(idx, node)                                                                \
    {                                                                                              \
        .behavior_dev = DT_LABEL(DT_INST_PHANDLE_BY_IDX(node, bindings, idx)),                     \
        .param1 = COND_CODE_0(DT_INST_PHA_HAS_CELL_AT_IDX(node, bindings, idx, param1), (0),       \
                              (DT_INST_PHA_BY_IDX(node, bindings, idx, param1))),                  \
        .param2 = COND_CODE_0(DT_INST_PHA_HAS_CELL_AT_IDX(node, bindings, idx, param2), (0),       \
                              (DT_INST_PHA_BY_IDX(node, bindings, idx, param2))),                  \
    }

#define KP_INST(n)                                                                                 \
    static struct behavior_meta_mod_config behavior_meta_mod_config_##n = {                      \
        .normal_binding = _TRANSFORM_ENTRY(0, n),                                                  \
        .morph_binding = _TRANSFORM_ENTRY(1, n),                                                   \
        .mods = DT_INST_PROP(n, mods),                                                             \
    };                                                                                             \
    static struct behavior_meta_mod_data behavior_meta_mod_data_##n = {};                        \
    DEVICE_DT_INST_DEFINE(n, behavior_meta_mod_init, NULL, &behavior_meta_mod_data_##n,          \
                          &behavior_meta_mod_config_##n, APPLICATION,                             \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_meta_mod_driver_api);

DT_INST_FOREACH_STATUS_OKAY(KP_INST)

#endif
