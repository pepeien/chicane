#!/bin/sh

cd .. && doxygen -q Doxyfile
python Documentation.py -i '.docs/xml' -o '.docs/references' -d 'Include/Chicane' -n 'Chicane' -s 'CHICANE_BOX,CHICANE_CORE,CHICANE_GRID,CHICANE_RUNTIME'