#define DT_DRV_COMPAT zmk_os_key

// Dependencies
#include <device.h>
#include <drivers/behavior.h>
#include <logging/log.h>
#include <zmk/behavior.h>

#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <zmk/os_mode.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct behavior_os_key_config {
    struct zmk_behavior_binding winlin_binding;
    struct zmk_behavior_binding osx_binding;
};

struct behavior_os_key_state {
    int pressed_os_mode;
};

// Initialization Function
static int os_key_init(const struct device *dev) { return 0; };

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
  const int active_mode = zmk_active_os_mode();
  const struct device *dev = device_get_binding(binding->behavior_dev);
  const struct behavior_os_key_config *cfg = dev->config;
    struct behavior_os_key_state *state = dev->data;

  if(active_mode == 0) {
    zmk_behavior_queue_add(event.position, cfg->winlin_binding, true, 100);
  } else {
    zmk_behavior_queue_add(event.position, cfg->osx_binding, true, 100);
  }

  state->pressed_os_mode = active_mode;
  return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
  const struct device *dev = device_get_binding(binding->behavior_dev);
  const struct behavior_os_key_config *cfg = dev->config;
    struct behavior_os_key_state *state = dev->data;

  if(state->pressed_os_mode == 0) {
    zmk_behavior_queue_add(event.position, cfg->winlin_binding, false, 100);
  } else {
    zmk_behavior_queue_add(event.position, cfg->osx_binding, false, 100);
  }
  
  return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_os_mode_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};


#define OS_KEY_INST(n)                                                                             \
    static struct behavior_os_key_config behavior_os_key_config_##n = {                             \
        .binding = ZMK_KEYMAP_EXTRACT_BINDING(idx, DT_DRV_INST(n))}                                \
    DEVICE_DT_INST_DEFINE(n, os_mode_init, NULL, NULL, &behavior_os_key_config_##n,                \
                          APPLICATION,                                                             \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_os_mode_driver_api);


DT_INST_FOREACH_STATUS_OKAY(OS_KEY_INST)
#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */