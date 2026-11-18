/*
 * Copyright (c) 2026 Marcos Chow Castro
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_switch_layer_behavior_switch_default_layer_mode

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define SWITCH_DEFAULT_LAYER_MODE_SOURCE 0
#define SWITCH_DEFAULT_LAYER_MODE_TARGET 1
#define SWITCH_DEFAULT_LAYER_MODE_TOGGLE 2

struct switch_default_layer_mode_config {
    uint8_t source_layer;
    uint8_t target_layer;
};

static uint8_t switch_default_layer_mode;

static const struct switch_default_layer_mode_config switch_default_layer_mode_cfg = {
    .source_layer = DT_INST_PROP(0, source_layer),
    .target_layer = DT_INST_PROP(0, target_layer),
};

static int switch_default_layer_mode_save(void) {
    int ret = settings_save_one("zmk_switch_default_layer/mode", &switch_default_layer_mode,
                                sizeof(switch_default_layer_mode));

    if (ret < 0) {
        LOG_WRN("Could not save switch default layer mode (%d).", ret);
    }

    return ret;
}

static int switch_default_layer_mode_apply(void) {
    uint8_t layer = switch_default_layer_mode == SWITCH_DEFAULT_LAYER_MODE_TARGET
                        ? switch_default_layer_mode_cfg.target_layer
                        : switch_default_layer_mode_cfg.source_layer;

    return zmk_keymap_layer_to(layer, false);
}

static int switch_default_layer_mode_set(const char *name, size_t len, settings_read_cb read_cb,
                                         void *cb_arg) {
    const char *next;

    if (settings_name_steq(name, "mode", &next) && !next) {
        if (len != sizeof(switch_default_layer_mode)) {
            return -EINVAL;
        }

        int ret = read_cb(cb_arg, &switch_default_layer_mode, sizeof(switch_default_layer_mode));
        if (ret >= 0 && switch_default_layer_mode > SWITCH_DEFAULT_LAYER_MODE_TARGET) {
            switch_default_layer_mode = SWITCH_DEFAULT_LAYER_MODE_SOURCE;
        }

        return ret;
    }

    return -ENOENT;
}

static struct settings_handler switch_default_layer_mode_conf = {
    .name = "zmk_switch_default_layer",
    .h_set = switch_default_layer_mode_set,
};

static int switch_default_layer_mode_init(void) {
    settings_subsys_init();

    int ret = settings_register(&switch_default_layer_mode_conf);
    if (ret < 0) {
        LOG_ERR("Could not register switch default layer settings (%d).", ret);
        return ret;
    }

    settings_load_subtree("zmk_switch_default_layer");

    return switch_default_layer_mode_apply();
}
SYS_INIT(switch_default_layer_mode_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static int behavior_switch_default_layer_mode_init(const struct device *dev) { return 0; }

static int on_switch_default_layer_mode_pressed(struct zmk_behavior_binding *binding,
                                                struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
    case SWITCH_DEFAULT_LAYER_MODE_SOURCE:
    case SWITCH_DEFAULT_LAYER_MODE_TARGET:
        switch_default_layer_mode = binding->param1;
        break;
    case SWITCH_DEFAULT_LAYER_MODE_TOGGLE:
        switch_default_layer_mode = switch_default_layer_mode == SWITCH_DEFAULT_LAYER_MODE_TARGET
                                        ? SWITCH_DEFAULT_LAYER_MODE_SOURCE
                                        : SWITCH_DEFAULT_LAYER_MODE_TARGET;
        break;
    default:
        return -EINVAL;
    }

    int ret = switch_default_layer_mode_save();
    if (ret < 0) {
        return ret;
    }

    return switch_default_layer_mode_apply();
}

static int on_switch_default_layer_mode_released(struct zmk_behavior_binding *binding,
                                                 struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_switch_default_layer_mode_driver_api = {
    .binding_pressed = on_switch_default_layer_mode_pressed,
    .binding_released = on_switch_default_layer_mode_released,
};

BEHAVIOR_DT_INST_DEFINE(0, behavior_switch_default_layer_mode_init, NULL, NULL, NULL, POST_KERNEL,
                        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_switch_default_layer_mode_driver_api);
