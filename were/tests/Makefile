all: sparkle

sparkle:
	mkdir -p build
	cd build; cmake ..; make -j2
	strip ./build/test1
	stat ./build/test1 | grep Size:

clean:
	rm -rf build

