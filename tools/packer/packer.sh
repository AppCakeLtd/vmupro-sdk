#!/bin/sh

#
# Creates the python venv and passes the build params
# 2025, 8BM
#

# c:\myprojects\vmusdk\examples\minimal
PROJECTDIR="$1"
# "vmupro_minimal" (matches your .elf filename in projectdir/build/<yourfile>.app.elf)
ELFNAME="$2"
# usually "icon.bmp"
ICON="$3"
# usually "metadata.json"
META="$4"
# set to true for extra spam
DEBUG="$5"

# Need the tool's folder path to properly locate requirements.txxt
# i.e. the location where THIS script resides
# relative/abs pathssupported
SCRIPT_PATH="$0"
while [ -L "$SCRIPT_PATH" ]; do
    SCRIPT_PATH="$(readlink "$SCRIPT_PATH")"
done
SCRIPT_DIR="$(cd "$(dirname "$SCRIPT_PATH")" && pwd)"
TOOLDIR="$SCRIPT_DIR"
echo "Packer tool's dir: $TOOLDIR"

# if [ -z "$PROJECTDIR" ]; then
#     echo "Inferring projectdir..."
#     PROJECTDIR="$(pwd)"
# fi

echo "Project's dir: $PROJECTDIR"

VENVNAME="vmupacker_venv"
VENVDIR="$PROJECTDIR/$VENVNAME"

echo "VMUPacker: checking virtual environment"
if [ -z "$VIRTUAL_ENV" ]; then
    echo "VMUPacker: creating python3 venv @ $VENVDIR"
    python3 -m venv "$VENVDIR"
    if [ $? -ne 0 ]; then
        echo "Failed to create venv"
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
--elfname "$ELFNAME" \
--icon "$ICON" \
--meta "$META" \
--sdkversion 1.0.0 \
--debug "$DEBUG"

echo "closing the python vmupacker venv"
deactivate
