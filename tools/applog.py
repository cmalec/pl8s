#!/usr/bin/env python3
"""Extract APP_LOG lines from a pebble log capture."""
import re
import sys

for line in open(sys.argv[1]):
    if "main.c" in line and "QemuInboundPacket" not in line:
        print(line, end="")
