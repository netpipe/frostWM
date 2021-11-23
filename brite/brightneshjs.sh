#! /usr/bin/env bash
set -e

# Help
if [[ $# -eq 0 ]] ; then
    cat <<EOF
Set the brightness of the specified display equal to the primary display
Usage: $(basename "$0") <output-name> [brightness-offset]
Example: $(basename "$0") DP-1 +20

EOF
    exit 255
fi

# Arguments
secondary_display_name="$1"
brightness_offset="${2:-+0}"

# Primary display
primary_display="/sys/class/backlight/intel_backlight"
actual_brightness=$(cat $primary_display/actual_brightness)
max_brightness=$(cat $primary_display/max_brightness)

# Calculate brightness
frac=$(printf %.2f\\n "$(( 100 * $actual_brightness / $max_brightness + $brightness_offset  ))e-2")
echo "Setting $secondary_display_name brightness: $frac"

# Set brightness
xrandr --output "$secondary_display_name" --brightness $frac

