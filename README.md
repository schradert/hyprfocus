# hyprfocus

a plugin which provides focus animations for us borderless folks, originally inspired by [flashfocus](https://github.com/fennerm/flashfocus)!
Modified to work with the latest Hyprland and support hyprpm as well as some other improvements.

## animations

flash

![preview](flash.gif)

shrink

![preview](shrink.gif)

## installation

instructions based on [the official wiki](https://wiki.hyprland.org/Plugins/Using-Plugins/#compiling-official-plugins)

```
hyprpm add https://github.com/hyprfocus
```

## getting started

to start using hyprfocus, add this to your hyprland config:

```
plugin:hyprfocus {
    enabled = yes

    focus_animation = flash

    bezier = bezIn, 0.5,0.0,1.0,0.5
    bezier = bezOut, 0.0,0.5,0.5,1.0

    flash {
        flash_opacity = 0.7

        in_bezier = bezIn
        in_speed = 0.5

        out_bezier = bezOut
        out_speed = 3
    }

    shrink {
        shrink_percentage = 0.8

        in_bezier = bezIn
        in_speed = 0.5

        out_bezier = bezOut
        out_speed = 3
    }
}
```

### plugin configuration

`enabled` (yes/no) -> enable or disable the plugin

`focus_animation` (flash/shrink/none) -> animation for keyboard-driven focus

### animations configuration

animations can be configured within the plugin scope with the following syntax:

```
plugin:hyprfocus {
    <animation> {

    }
}
```

### shared animation variables

`in_bezier` (bezier) -> bezier curve towards the animation apex

`out_bezier` (bezier) -> bezier curve towards the default window state

`in_speed` (float) -> speed for the 'in' bezier

`out_speed` (float) -> speed for the 'out' bezier

### flash

`flash_opacity` (float) -> opacity to during the flash's apex

### shrink

`shrink_percentage` (float) -> the amount a window has shrunk during the animation's apex

### dispatching

hyprfocus can also flash the currently focused window through the `animatefocused` dispatcher:

```
bind = $mod, space, animatefocused
```

# Special thanks

- [flashfocus](https://github.com/fennerm/flashfocus): An earlier project of similar nature
- [Original repo](https://github.com/VortexCoyote/hyprfocus.git)
