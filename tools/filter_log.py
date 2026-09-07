#!/usr/bin/env python3
"""Filter emulator log lines, hiding noisy pkjs warnings."""
import re
import sys

for line in open(sys.argv[1]):
    if "QemuInboundPacket" in line:
        continue
    if line.strip():
        print(line, end="")
