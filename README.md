# zmk-switch-layer

ZMK module to persistently switch layers. There is no need for a physical switch and no need to flash again (for example like the logitech keyboards that activate the macos and windows layer), the change always remains active, after reboot, after flash and after power off. The keyboard can toggle the base layer other any layer while using the keyboard.

It includes two independent behaviors:
- `switch-layer-mode`: activates an alternative layer above another without changing the default layer
- `switch-default-layer-mode`: changes the persistent default layer between two options

## Features

| Feature | Support | Note |
|---|---:|---|
| Keeps the mode after power off or reboot | Yes | Uses ZMK settings: `zmk_switch_layer/mode`. |
| Changes the persistent default layer | No | Does not call `zmk_keymap_layer_to()`. |
| Activates one layer above another | Yes | Activates `target-layer` when `source-layer` is active and the stored mode is `target`. |
| Restores when leaving `source-layer` | Yes | Deactivates `target-layer` if it was activated by this module. |
| Allows returning manually to normal mode | Yes | Use `&sw_layer 0`. |
| Allows toggling from one key | Yes | Use `&sw_layer 2`. |
| Requires layers to be base or layer `0` | No | They can be any pair of layers from the keymap. |
| Can activate a layer that is also base/default | Yes | Activates it as a normal layer; does not store it as default. |
| Multiple layer pairs | Partial | Multiple nodes can be defined, but they share the same global persistent mode. |

## Versions, branches and tags

All my ZMK projects follow the same rules. The idea is that every project can be paired with ZMK in its different versions (v0.3, v0.4, ...), so you always know which branch or tag fits the ZMK version you use.

What to write as `revision:` in your `west.yml`:

| You want | `revision:` |
|---|---|
| The latest stable version | `main` |
| Stay on ZMK v0.3 and keep receiving fixes | `zmk-v0.3` |
| A fixed version that never changes | a tag, for example `v0.0.3` |

Branches:

- `main`: the stable version. It follows the newest ZMK version that the project supports.
- `zmk-v0.3`: maintenance for ZMK v0.3. It only receives fixes, never new features. When the project moves to a newer ZMK, the next branch is created the same way (`zmk-v0.4`, ...).
- `dev`: development. It can be broken at any time, do not use it for your keyboard.

Tags:

- `v0.0.3`: the common starting point of all my ZMK projects (Nov 18, 2026), tested with ZMK v0.3.
- `v0.0.4`, `v0.0.5`, ...: new versions, released from `main`. `v0.0.4` is planned as the update to the newest ZMK.
- `v0.0.3.1`, `v0.0.3.2`, ...: fixes for the ZMK v0.3 line, released from `zmk-v0.3` (`v0.0.3.1-fix` while a fix is still being tested).

Every release says which ZMK version it was tested with. The full list of changes is in [CHANGELOG.txt](CHANGELOG.txt).

## Add As External Module

Add it to `west.yml`:

`CONFIG_SETTINGS` must be enabled.

```yaml
manifest:
  remotes:
    - name: mctechnology17
      url-base: https://github.com/mctechnology17
  projects:
    - name: zmk-switch-layer
      remote: mctechnology17
      revision: main
  self:
    path: config
```

Compatible:

```txt
zmk-switch-layer,behavior-switch-layer-mode
zmk-switch-layer,behavior-switch-default-layer-mode
```

Kconfig:

```txt
CONFIG_ZMK_SWITCH_LAYER_BEHAVIOR
CONFIG_ZMK_SWITCH_DEFAULT_LAYER_BEHAVIOR
```

Local build:

```sh
-DZMK_EXTRA_MODULES="/path/to/zmk-switch-layer"
```


## switch-default-layer-mode

`switch-default-layer-mode` changes the default layer and keeps the mode after power off or reboot.

Example:

```dts
/ {
    behaviors {
        /omit-if-no-ref/ sw_default: switch_default_layer_mode {
            compatible = "zmk-switch-layer,behavior-switch-default-layer-mode";
            #binding-cells = <1>; /* 0: source, 1: target, 2: toggle */
            source-layer = <_DEFAULT>;
            target-layer = <_WINDOWS>;
        };
    };
};
```

Usage:

```dts
&sw_default 1  /* use target-layer as default */
&sw_default 0  /* return to source-layer as default */
&sw_default 2  /* toggle */
```

## switch-layer-mode

Define the behavior in the keymap:

```dts
/ {
    behaviors {
        /omit-if-no-ref/ sw_layer: switch_layer_mode {
            compatible = "zmk-switch-layer,behavior-switch-layer-mode";
            #binding-cells = <1>; /* 0: source, 1: target, 2: toggle */
            source-layer = <_SYMBOL>;
            target-layer = <_LINUX>;
        };
    };
};
```

Add control keys:

```dts
&sw_layer 1  /* use target-layer when source-layer is active */
&sw_layer 0  /* return to source-layer */
&sw_layer 2  /* toggle */
```


## Multiple switch-layer-mode

The behavior is not limited to one layer or one pair. You can define multiple nodes with the same compatible and change the `source-layer` / `target-layer` pair in each one:

```dts
/ {
    behaviors {
        sw_symbol_linux: switch_symbol_linux {
            compatible = "zmk-switch-layer,behavior-switch-layer-mode";
            #binding-cells = <1>;
            source-layer = <_SYMBOL>;
            target-layer = <_LINUX>;
        };

        sw_nav_game: switch_nav_game {
            compatible = "zmk-switch-layer,behavior-switch-layer-mode";
            #binding-cells = <1>;
            source-layer = <_NAV>;
            target-layer = <_GAME>;
        };
    };
};
```

Usage:

```dts
&sw_symbol_linux 2
&sw_nav_game 2
```

## Related Topics and short comparison

| Module / feature | What it does | Persistent | Changes default layer |
|---|---|---:|---:|
| `switch-layer-mode` | When `source-layer` is active, decides whether to mount `target-layer` above it according to a stored mode. | Yes | No |
| `switch-default-layer-mode` | Changes the default layer between `source-layer` and `target-layer` according to a stored mode. | Yes | Yes |
| [`zmk-auto-layer`](https://github.com/urob/zmk-auto-layer) | Activates a temporary layer and keeps it active while keys allowed by a `continue-list` are pressed. | No | No |
| [elpekenin's `default-layer`](https://github.com/elpekenin/zmk-userspace) | Changes the default layer and stores it. | Yes | Yes |
| [ZMK layer behaviors](https://zmk.dev/docs/keymaps/behaviors/layers) | `&mo`, `&tog`, `&to` and layer-tap activate, toggle or change layers from a key. | No | `&to` can change the active base layer at runtime, but does not store it |
| [ZMK conditional layers](https://zmk.dev/docs/keymaps/conditional-layers) | Activates a layer when a set of other layers is already active. | No | No |
| [ZMK persistent settings](https://zmk.dev/docs/config/settings) | Infrastructure for storing state in flash. Not a layer behavior by itself. | Yes | No |

## Credit

This module is based on [elpekenin/zmk-userspace](https://github.com/elpekenin/zmk-userspace) and his great work on persistent ZMK behaviors.

This implementation is separated as `zmk-switch-layer`, with its own compatible, Kconfig, settings and sources.

## Difference from elpekenin's default-layer behavior

elpekenin's `default-layer` behavior changes the persistent default layer.

`switch-layer-mode` does not change the default layer. It stores a persistent mode and, when `source-layer` is active, decides whether `target-layer` should also be activated above it.

`switch-default-layer-mode` does change the persistent default layer, but it stays separate so it can be used or removed without affecting `switch-layer-mode`.

