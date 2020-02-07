#!/bin/sh

find . -type f -name "*.h"   -print0 | xargs -0 sed -i'' -e "s/${1?}/${2?}/g"
find . -type f -name "*.c"   -print0 | xargs -0 sed -i'' -e "s/${1?}/${2?}/g"
find . -type f -name "*.cpp" -print0 | xargs -0 sed -i'' -e "s/${1?}/${2?}/g"

