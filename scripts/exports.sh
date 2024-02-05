#!/bin/sh
# Print exported symbols for a DLL on Windows
# from: https://nullprogram.com/blog/2021/05/31/
set -e
printf 'LIBRARY %s\nEXPORTS\n' "$1"
objdump -p "$1" | awk '/^$/{t=0} {if(t)print$NF} /^\[O/{t=1}'

