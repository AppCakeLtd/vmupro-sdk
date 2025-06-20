#!/bin/sh
python3 ../../tools/packer/send.py --func send --localfile "vmupro_minimal.vmupack" --remotefile "apps/vmupro_minimal.vmupack" --comport /dev/ttyYOURPATH --exec True