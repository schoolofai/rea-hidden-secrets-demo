#!/usr/bin/env bash
# Runs the REA CLI steps from the video against YOUR OWN build of tinyvault.
set -euo pipefail
B=${1:-./tinyvault}
echo "\$ strings $B | grep -c TV_GHOST"; strings "$B" | grep -c TV_GHOST || true
echo "\$ rea decompile $B FUN_001010a0 --provider ghidra"
npx rea decompile "$B" FUN_001010a0 --provider ghidra --format json | python3 -c "import json,sys;print(json.load(sys.stdin)['raw_result']['value'])"
echo "\$ rea read-bytes $B 0x1020d0 --length 15 --provider ghidra"
npx rea read-bytes "$B" 0x1020d0 --length 15 --provider ghidra --filter-output raw_result.bytes_hex
