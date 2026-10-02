# PrismWatt Smart Hub

## Dashboard

The 160x128 display shows a static Smart Hub dashboard with seven OFF channel
cards and a SET card. It uses the extracted 14px Google Sans LVGL font;
the title and card labels use the same 14px size. Channel cards are 34x51px
with 6px rounded corners and a 2px rounded primary-color focus ring. All eight
cards share the same size and aligned columns. Card and background colors use
the supplied Material 3 palette. Rounded card edges use 4x4 subpixel coverage,
and bitmap icons use bilinear alpha scaling for smoother edges.

The four SVG assets in `temp/images` are embedded as 24x24 4bpp alpha bitmaps
and scaled to fit their cards. Font extraction details are in
[temp/README.md](./temp/README.md).

- Build the project

```shell
west build -b stm32_min_dev/stm32f103x8
```

- Build and run the native SDL UI simulator

The SDL input backend supports desktop mouse/touch events. Keyboard input is not
provided by this Zephyr SDL backend.

```shell
west build -b native_sim/native/64 -p always -- -DCONF_FILE="prj.conf;prj_native_sim.conf"
west build -t run
```