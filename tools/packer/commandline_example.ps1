# Note: example only
# - assumes it's being called from the packer directory
# - installs requirements into active python instance (no venv)

pip install -r requirements.txt

# package the minimal example
# accepts relative or absolute paths
py .\packer.py  --projectdir "..\..\examples\minimal" --elfname vmupro_minimal --sdkversion "1.0.0" --meta ./metadata.json --icon icon.bmp --debug true


