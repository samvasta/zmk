#define DT_DRV_COMPAT zmk_os_mode

// Dependencies
#include <device.h>
#include <drivers/behavior.h>
#include <logging/log.h>

#include <zmk/behavior.h>
#include <zmk/event_manager.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

// Instance-Unique Data Struct (Optional)
struct behavior_os_mode_data {
    bool example_data_param1;
    bool example_data_param2;
    bool example_data_param3;
};

// Instance-Unique Config Struct (Optional)
struct behavior_os_mode_config {
    bool example_config_param1;
    bool example_config_param2;
    bool example_config_param3;
};

// Initialization Function
static int os_mode_init(const struct device *dev) {
    return 0;
};

// API Structure
static const struct behavior_driver_api os_mode_driver_api = {

};

static int os_mode_position_state_changed_listener(const zmk_event_t *eh);

ZMK_LISTENER(behavior_os_mode, os_mode_position_state_changed_listener);
ZMK_SUBSCRIPTION(behavior_os_mode, zmk_position_state_changed);

static int os_mode_position_state_changed_listener(const zmk_event_t *eh){
    // Do stuff...
}

DEVICE_DT_INST_DEFINE(0,                                                    // Instance Number (Equal to 0 for behaviors that don't require multiple instances,
                                                                            //                  Equal to n for behaviors that do make use of multiple instances)
                      os_
                    mode_init, NULL,                           // Initialization Function, Power Management Device Pointer
                      &os_
                    mode_data, &os_
                    mode_config,       // Behavior Data Pointer, Behavior Configuration Pointer (Both Optional)
                      APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,     // Initialization Level, Device Priority
                      &os_
                    mode_driver_api);                         // API Structure

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */