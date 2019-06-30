
all: debug
install: install-debug

debug:
	./gradlew assembleDebug

release:
	./gradlew assembleRelease

install-debug: debug
	adb uninstall com.sion.sparkle > /dev/null 2>&1
	adb install app/build/outputs/apk/debug/app-debug.apk

install-release: release
	adb uninstall com.sion.sparkle > /dev/null 2>&1
	adb install app/build/outputs/apk/release/app-release.apk

uninstall:
	adb uninstall com.sion.sparkle > /dev/null 2>&1
clean:
	rm -rf .gradle/ app/.externalNativeBuild/ app/build/ build/

