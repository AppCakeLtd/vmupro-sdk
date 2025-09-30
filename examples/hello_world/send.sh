#!/bin/sh
# example: To use existing win py and COM ports under WSL:
# sudo ln -sf /mnt/c/dev/python3_10_6/python.exe /usr/local/bin/python
# rm the symlink to revert
python ../../tools/packer/send.py --func send --localfile "hello_world.vmupack" --remotefile "apps/hello_world.vmupack" --exec --monitor