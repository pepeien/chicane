#!/bin/bash

SCRIPT_DIR=$(dirname "$(realpath "$0")")

cd $SCRIPT_DIR/Linux && docker compose --verbose up --build