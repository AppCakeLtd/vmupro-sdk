echo "VMUPacker: checking virtual environment"
if [ -z "$VIRTUAL_ENV" ]; then
    echo "VMUPacker: creating python3 venv"
    python3 -m venv ".venv"
    if [ $? -ne 0 ]; then
        echo "Failed to create venv"
        exit 1
    fi
else
    echo "Already inside a virtual environment: $VIRTUAL_ENV"
    echo "Type 'deactivate' to start a fresh environment"
    exit 0
fi

# Activate the virtual environment
. ".venv/bin/activate"

pip install -r "requirements.txt"

# package the minimal example
# accepts relative or absolute paths
python "packer.py" \
    --basedir "../../examples/minimal" \
    --elfname "vmupro_minimal" \
    --icon "icon.bmp" \
    --meta "metadata.json" \
    --sdkversion 0.0.1 \
    --debug "true"

echo "Closing the python environment"
# Optional manual deactivate
# deactivate
