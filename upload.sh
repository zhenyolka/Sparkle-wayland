#!/bin/sh

set -e

read -s -p "Password? " PASS
echo

for FILE_NAME in upload/*
do
    curl -T "${FILE_NAME}" ftp://mogryph:${PASS}@ftp.drivehq.com/sparkle/
done

