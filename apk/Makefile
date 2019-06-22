
all: debug

debug:
	./gradlew assembleDebug

install: debug
	adb uninstall com.sion.sparkle > /dev/null 2>&1
	adb install app/build/outputs/apk/debug/app-debug.apk

uninstall:
	adb uninstall com.sion.sparkle > /dev/null 2>&1
clean:
	rm -rf ./build ./app/build ./app/.externalNativeBuild ./.gradle

