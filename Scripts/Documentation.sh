#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "${ROOT_DIR}"

XML_INDEX=".docs/xml/index.xml"

if [[ "${SKIP_DOXYGEN:-0}" == "1" ]]; then
  if [[ ! -f "${XML_INDEX}" ]]; then
    echo "error: SKIP_DOXYGEN=1 but ${XML_INDEX} was not found" >&2
    exit 1
  fi
elif command -v doxygen >/dev/null 2>&1; then
  doxygen -q Doxyfile
elif [[ -f "${XML_INDEX}" ]]; then
  echo "doxygen not found; reusing existing ${XML_INDEX}"
else
  echo "error: doxygen not found and ${XML_INDEX} is missing" >&2
  exit 1
fi

python3 Scripts/Documentation/Helper.py \
  -i .docs/xml \
  -o References \
  -d Includes/Chicane/ \
  -H Includes/Chicane \
  -n Chicane \
  -s CHICANE_BOX,CHICANE_CORE,CHICANE_DRIFT,CHICANE_GRID,CHICANE_KERB,CHICANE_RENDERER,CHICANE_RUNTIME,CHICANE_SCREECH
