#!/bin/sh

adb root > /dev/null 2>&1
adb push user.lua /data/data/com.sion.sparkle/
adb push sparkle.lua /data/data/com.sion.sparkle/
adb shell "chmod 0644 /data/data/com.sion.sparkle/user.lua"
adb shell "chmod 0644 /data/data/com.sion.sparkle/sparkle.lua"
