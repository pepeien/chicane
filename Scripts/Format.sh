#!/bin/bash

cd ..
find . \
-type d \( -name Vendor \) \
-prune -o \
-type f \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) \
-exec clang-format \
-i {} +