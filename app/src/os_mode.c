
#include <device.h>
#include <init.h>
#include <zmk/keys.h>
#include <settings/settings.h>

#if IS_ENABLED(CONFIG_SETTINGS)

#include <settings/settings.h>

#endif

#include <logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/keys.h>
#include <zmk/event_manager.h>
#include <zmk/events/os_mode_changed.h>

static uint8_t active_mode;

static void raise_active_os_mode_changed_event() {
    ZMK_EVENT_RAISE(new_zmk_active_os_mode_changed(
        (struct zmk_active_os_mode_changed){.current_mode = active_mode}));
}

static void raise_active_os_mode_changed_event_callback(struct k_work *work) {
    raise_active_os_mode_changed_event();
}

K_WORK_DEFINE(raise_active_os_mode_changed_event_work, raise_active_os_mode_changed_event_callback);

void set_active_os_mode(uint8_t active_mode) {
    char setting_name[15];

    sprintf(setting_name, "osmode/active", active_mode);
    LOG_DBG("Setting active os mode to %s", log_strdup(active_mode));
    settings_save_one(setting_name, active_mode, sizeof(uint8_t));
    k_work_submit(&raise_active_os_mode_changed_event_work);
}

static int ble_save_profile() {
#if IS_ENABLED(CONFIG_SETTINGS)
    return k_work_reschedule(&ble_save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
#else
    return 0;
#endif
}

int zmk_os_mode_select(uint8_t mode) {
    if (mode < 0 || mode >= 2) {
        return -ERANGE;
    }

    LOG_DBG("selecting os mode %d", mode);
    if (active_mode == mode) {
        return 0;
    }

    active_mode = mode;
    ble_save_profile();

    raise_active_os_mode_changed_event();

    return 0;
};

int zmk_os_mode_a() { return zmk_os_mode_select(0); }

int zmk_os_mode_b() { return zmk_os_mode_select(1); }

int zmk_os_mode_toggle() { return zmk_os_mode_select(active_mode ? 0 : 1); }

int zmk_active_os_mode() { return active_mode; }