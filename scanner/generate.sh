#!/bin/sh

rm ../generated/* > /dev/null 2>&1

./scanner /usr/share/wayland/wayland.xml ../generated/ \
wl_compositor \
wl_surface \
wl_callback \
wl_region \
wl_output \
wl_seat \
wl_pointer \
wl_keyboard \
wl_touch \
wl_shell \
wl_shell_surface \
