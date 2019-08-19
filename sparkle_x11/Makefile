all: sparkle

sparkle:
	mkdir -p build
	cd build; cmake ..; make -j2
	stat build/sparkle | grep Size:
	strip build/sparkle
	stat build/sparkle | grep Size:

clean:
	rm -rf build

