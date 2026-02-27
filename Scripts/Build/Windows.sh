#!/bin/bash

SCRIPT_DIR=$(dirname "$(realpath "$0")")

cd $SCRIPT_DIR/Windows && docker compose --verbose up --build