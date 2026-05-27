#!/bin/bash
cd "$(dirname "$0")"
export TERM="${TERM:-xterm-256color}"
./console_fnaf
echo ""
read -r -p "Game closed. Press Enter to exit."
