#!/bin/sh

set -e

make -C ./pcm_sparkle clean > /dev/null 2>&1
tar -czf upload/pcm_sparkle.tar.gz ./pcm_sparkle



read -s -p "Password? " PASS
echo

for FILE_NAME in upload/*
do
    echo "Uploading ${FILE_NAME}"
    curl -T "${FILE_NAME}" ftp://mogryph:${PASS}@ftp.drivehq.com/sparkle/
    echo "Ok"
done

