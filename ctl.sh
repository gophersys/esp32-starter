#!/usr/bin/env bash
# ctl.sh — build/flash/monitor for this app, with deterministic port selection.
#
#   ./ctl.sh build              compile (board: $BOARD)
#   ./ctl.sh flash   [slot]     flash the board in hub slot N   (default: $SLOT)
#   ./ctl.sh monitor [slot]     serial monitor on hub slot N    (exit: Ctrl+])
#   ./ctl.sh run     [slot]     build + flash + monitor
#   ./ctl.sh ports              show which board is in which slot
#
# Slots are PHYSICAL hub positions (stable across reboots/replugs):
#   1-2 = ESP32 (CP2102)   3-4 = Nucleo H743ZI (ST-Link)
# A per-slot lock makes flash & monitor mutually exclusive, so a forgotten
# monitor can never brick a flash attempt with "port busy" weirdness.
set -euo pipefail
cd "$(dirname "$0")"

SLOT="${2:-${SLOT:-2}}"
BOARD="${BOARD:-esp32_devkitc_wroom/esp32/procpu}"

port() {
  local p
  p=$(ls /dev/serial/by-path/*-usb-0:"${SLOT}":* 2>/dev/null | head -1) \
    || { echo "no serial device in slot ${SLOT}" >&2; exit 1; }
  echo "$p"
}

locked() {  # hold the slot lock for the duration of "$@"
  exec 9>"/tmp/mcu-slot-${SLOT}.lock"
  flock -n 9 || { echo "slot ${SLOT} is busy — a monitor or flash is already using it (close it first)" >&2; exit 1; }
  "$@"
}

case "${1:-}" in
  build)   west build -b "$BOARD" . ;;
  flash)   locked west flash --esp-device "$(port)" ;;
  monitor) locked west espressif monitor -p "$(port)" ;;
  run)     west build -b "$BOARD" . && locked bash -c "west flash --esp-device $(port) && west espressif monitor -p $(port)" ;;
  ports)   for n in 1 2 3 4 5 6; do p=$(ls /dev/serial/by-path/*-usb-0:$n:* 2>/dev/null | head -1) || true; [ -n "${p:-}" ] && echo "slot $n: $p"; p=""; done ;;
  *)       grep "^#   " "$0" | sed "s/^#   //"; exit 1 ;;
esac
