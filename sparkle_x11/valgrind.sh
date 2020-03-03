#!/bin/sh

valgrind --leak-check=full --show-leak-kinds=all ./build/sparkle

