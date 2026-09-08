#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${ROOT_DIR}"

doxygen -q Doxyfile
python3 Scripts/Documentation/Helper.py \
  -i .docs/xml \
  -o References \
  -d Includes/Chicane/ \
  -H Includes/Chicane \
  -n Chicane \
  -s CHICANE_BOX,CHICANE_CORE,CHICANE_DRIFT,CHICANE_GRID,CHICANE_KERB,CHICANE_RENDERER,CHICANE_RUNTIME,CHICANE_SCREECH
