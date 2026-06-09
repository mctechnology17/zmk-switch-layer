# Copyright (c) 2026 Marcos Chow Castro
# SPDX-License-Identifier: MIT

zephyr_library()

# ### zmk-switch-layer START ###
zephyr_include_directories(${APPLICATION_SOURCE_DIR}/include)

if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  zephyr_library_sources_ifdef(CONFIG_ZMK_SWITCH_LAYER_BEHAVIOR src/behaviors/behavior_switch_layer_mode.c)
  zephyr_library_sources_ifdef(CONFIG_ZMK_SWITCH_DEFAULT_LAYER_BEHAVIOR src/behaviors/behavior_switch_default_layer_mode.c)
endif()
# ### zmk-switch-layer END ###
