#!/bin/bash

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT" || exit 1

doxygen -q Doxyfile
python3 Scripts/Documentation/Helper.py \
-i .docs/xml \
-o References \
-d Includes/Chicane/ \
-H Includes/Chicane \
-n Chicane \
-s CHICANE_BOX,CHICANE_CORE,CHICANE_DRIFT,CHICANE_GRID,CHICANE_KERB,CHICANE_RENDERER,CHICANE_RUNTIME,CHICANE_SCREECH