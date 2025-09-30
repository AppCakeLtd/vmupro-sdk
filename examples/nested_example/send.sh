#!/bin/sh
# example: To use existing win py and COM ports under WSL:
# sudo ln -sf /mnt/c/dev/python3_10_6/python.exe /usr/local/bin/python
# rm the symlink to revert
python ../../tools/packer/send.py --func send --localfile "nested_example.vmupack" --remotefile "apps/nested_example.vmupack" --exec --monitor