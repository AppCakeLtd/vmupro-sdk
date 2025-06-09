#!/bin/bash
echo "VMUPacker: creating python3 venv"
python3 -m venv .venv || exit 1
source .venv/bin/activate
pip install -r requirements.txt
echo "VMUPacker env created"
echo "VMUPacker: you can activate manually via .venv/Scripts/activate"
echo "Ready"