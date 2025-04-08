#!/usr/bin/env bash

make uninstall
make install
python -m pytest unittests/test_numc.py