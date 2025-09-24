#!/bin/sh

#
# Creates the python venv and passes the build params
# 2025, 8BM
#

# Need the tool's folder path to properly locate requirements.txt
# i.e. the location where THIS script resides
# relative/abs paths supported
SCRIPT_PATH="$0"
while [ -L "$SCRIPT_PATH" ]; do
    SCRIPT_PATH="$(readlink "$SCRIPT_PATH")"
done
SCRIPT_DIR="$(cd "$(dirname "$SCRIPT_PATH")" && pwd)"
TOOLDIR="$SCRIPT_DIR"
echo "Packer tool's dir: $TOOLDIR"

# Projectdir is always current working directory
PROJECTDIR="$(pwd)"
echo "Project's dir: $PROJECTDIR"

VENVDIR="$PROJECTDIR/vmupacker_venv"

echo "VMUPacker: checking virtual environment"
if [ -z "$VIRTUAL_ENV" ]; then
    echo "VMUPacker: creating python3 venv @ $VENVDIR"
    python3 -m venv "$VENVDIR"
    if [ $? -ne 0 ]; then
        echo "Failed to create venv"
        echo "Hint: VSCode may be using the vmupacker_venv directory"
        exit 1
    fi
else
    echo "Already inside a virtual environment: $VIRTUAL_ENV"
fi

# source $PROJECTDIR/vmupacker_venv/bin/activate
. "$VENVDIR/bin/activate"

# Add crcmod, pillow, etc
pip3 install -r "$TOOLDIR/requirements.txt"

echo "VMUPacker env created"
echo "Ready!"

# Running `which python` should point at the ESP IDF's python
# this will be automatic if you've ran the idf export script before building

python3 "$TOOLDIR/packer.py" \
  --projectdir "$PROJECTDIR" \
  --sdkversion "1.0.0" \
  "$@"

echo "closing the python vmupacker venv"
deactivate
