#!/bin/sh
python3 ../../tools/packer/send.py --func send --localfile "vmupro_minimal.vmupack" --remotefile "test.vmupack" --comport /dev/ttyYOURPATH --exec True