# Note: example only
# - assumes it's being called from the packer directory
# - installs requirements into active python instance (no venv)
#   (so may require ESP IDF import or sudo depending on your config)

pip install -r "requirements.txt"

# package the minimal example
# projectdir can be relative to cwd (../../somewhere) or absolute (c:\stuff\things)
# other params are relative to projectdir
python3 "packer.py" \
    --projectdir "../../examples/minimal" \
    --elfname "vmupro_minimal" \
    --icon "icon.bmp" \
    --meta "metadata.json" \
    --sdkversion 0.0.1 \
    --debug "true"
