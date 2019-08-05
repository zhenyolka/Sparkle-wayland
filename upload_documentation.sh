#!/bin/sh

set -e

read -s -p "Password? " PASS
echo

for FILE_NAME in documentation/output/russian/*
do
    echo "Uploading ${FILE_NAME}"
    curl -T "${FILE_NAME}" ftp://mogryph:${PASS}@ftp.drivehq.com/sparkle/documentation/russian/
    echo "Ok"
done

for FILE_NAME in documentation/output/english/*
do
    echo "Uploading ${FILE_NAME}"
    curl -T "${FILE_NAME}" ftp://mogryph:${PASS}@ftp.drivehq.com/sparkle/documentation/english/
    echo "Ok"
done

