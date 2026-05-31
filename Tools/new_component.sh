#!/bin/bash

echo -n "Enter component name (e.g., NewComponent): "
read COMP_NAME

# Get the directory of the current script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cmake -DCOMP_NAME="$COMP_NAME" -P "$SCRIPT_DIR/make_component.cmake"