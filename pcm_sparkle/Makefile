
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OUTPUT = libasound_module_pcm_sparkle.so

all: $(OUTPUT)

$(OUTPUT): $(SOURCES) $(HEADERS)
	$(CC) -Wall -Wextra -Wno-unused-parameter -O2 -fPIC -DPIC -shared -o $(OUTPUT) $(SOURCES)

clean:
	rm $(OUTPUT)

