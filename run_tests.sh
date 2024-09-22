#!/bin/bash

set -x
set -u

TESTS_DISK_DIR="/tmp"

rm -rf /tmp/*.disk
./build/tests/tests
rm -rf /tmp/*.disk
