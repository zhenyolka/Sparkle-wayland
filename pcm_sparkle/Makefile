
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OUTPUT = libasound_module_pcm_sparkle.so
INSTALL_DIR = $(shell pkgconf --variable=libdir alsa)

all: $(OUTPUT)

$(OUTPUT): $(SOURCES) $(HEADERS)
	$(CC) -Wall -Wextra -Wno-unused-parameter -O2 -fPIC -DPIC -shared \
	-o $(OUTPUT) $(SOURCES) -lasound

clean:
	rm -rf $(OUTPUT)

install:
	test -n "$(INSTALL_DIR)"
	mkdir -p $(INSTALL_DIR)/alsa-lib/
	cp $(OUTPUT) $(INSTALL_DIR)/alsa-lib/

uninstall:
	test -n "$(INSTALL_DIR)"
	rm $(INSTALL_DIR)/alsa-lib/$(OUTPUT)

pack: clean
	cd ..; tar -czf upload/pcm_sparkle.tar.gz ./pcm_sparkle

