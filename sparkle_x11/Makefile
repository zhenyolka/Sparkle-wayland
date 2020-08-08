all: sparkle

sparkle:
	mkdir -p build
	cd build; cmake ..; make -j16
	stat build/sparkle | grep Size:
	strip build/sparkle
	stat build/sparkle | grep Size:

check:
	mkdir -p build
	cd build; scan-build cmake ..; scan-build make -j16
	stat build/sparkle | grep Size:
	strip build/sparkle
	stat build/sparkle | grep Size:

clean:
	rm -rf build/*

