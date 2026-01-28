#!/usr/bin/env bash
set -euo pipefail

"/c/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
  "/c/dev/T66/T66.uproject" \
  -run=T66CoreDataRepair \
  -unattended -nop4 -NoLogTimes
