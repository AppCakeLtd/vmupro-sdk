#!/bin/sh
python3 ../../tools/packer/packer.py --projectdir . --appname nested_example --icon icon.bmp --meta ./metadata.json --sdkversion 1.0.0 --debug true
