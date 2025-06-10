#!/bin/sh

#
# Creates the python venv and passes the build params
# 2025, 8BM
#

# args:
# $1 = basedir
# $2 = elfname
# $3 = icon
# $4 = metadata
# $5 = debug

BASEDIR="$1"
ELFNAME="$2"
ICON="$3"
META="$4"
DEBUG="$5"

# Need the parent folder to create abs paths for pip, venv
# e.g. the location where THIS script resides
# should work even with relative path
SCRIPT_PATH="$0"
while [ -L "$SCRIPT_PATH" ]; do
  SCRIPT_PATH="$(readlink "$SCRIPT_PATH")"
done
echo "--------------------"
SCRIPT_DIR="$(cd "$(dirname "$SCRIPT_PATH")" && pwd)"
echo "$SCRIPT_DIR"

PARENTFOLDER="$SCRIPT_DIR"
echo "Packer dir: $PARENTFOLDER"

# if [ -z "$BASEDIR" ]; then
#     echo "Inferring basedir..."
#     BASEDIR="$(pwd)"
# fi
echo "Base dir: $BASEDIR"

echo "VMUPacker: checking virtual environment"
if [ -z "$VIRTUAL_ENV" ]; then
    echo "VMUPacker: creating python3 venv"
    python3 -m venv "$PARENTFOLDER/.venv"
    if [ $? -ne 0 ]; then
        echo "Failed to create venv"
        exit 1
    fi
else
    echo "Already inside a virtual environment: $VIRTUAL_ENV"
    echo "Type 'deactivate' to start a fresh environment"    
fi

# call .venv\Scripts\activate
. "$PARENTFOLDER/.venv/bin/activate"

# Add crcmod, pillow, etc
pip install -r "$PARENTFOLDER/requirements.txt"

echo "VMUPacker env created"
echo "Ready!"

# Running `where.exe python` should point at the ESP IDF's python
# this will be automatic if you've ran the idf export script before building
python "$PARENTFOLDER/packer.py" \
--basedir "$BASEDIR" \
--elfname "$ELFNAME" \
--icon "$ICON" \
--meta "$META" \
--sdkversion 1.0.0 \
--debug "$DEBUG"

echo "closing the python environment"
# Optional manual deactivate
# deactivate
