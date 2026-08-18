#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}/.."

if ! command -v clang-format >/dev/null 2>&1
then
    echo "clang-format was not found in PATH."
    exit 1
fi

find . \
    -type d \( -name Vendor \) -prune -o \
    -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) \
    -exec clang-format -i {} +

echo "Format complete."
