#!/bin/bash

set -x
set -u

TESTS_DISK_DIR="/tmp"

exec 2> test_output.log

rm -rf /tmp/*.disk
./build/tests/tests
rm -rf /tmp/*.disk
