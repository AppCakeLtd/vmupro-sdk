#!/bin/sh
python3 ../../tools/packer/packer.py --projectdir . --appname hello_world --icon icon.bmp --meta ./metadata.json --sdkversion 1.0.0 --debug true
