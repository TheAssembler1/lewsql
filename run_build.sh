#!/bin/bash

set -u
set -x

cmake -S . -B build
make -C build
