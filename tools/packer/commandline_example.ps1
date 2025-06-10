# get the venv running
. .\setup.ps1

# package the minimal example
py .\packer.py  --basedir ../../examples/minimal --elfname vmupro_minimal --sdkversion "0.0.1" --meta ./metadata.json --icon icon.bmp --debug true


