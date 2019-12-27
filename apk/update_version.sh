#!/bin/sh

VERSION_CODE=$(git rev-list --count HEAD)
VERSION_NAME="0.${VERSION_CODE}"

sed -i "s/^        versionCode = [a-zA-Z0-9.]\+$/        versionCode = ${VERSION_CODE}/g" app/build.gradle
sed -i "s/^        versionName = \"[a-zA-Z0-9.]\+\"$/        versionName = \"${VERSION_NAME}\"/g" app/build.gradle
