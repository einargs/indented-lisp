CC = clang-6.0
CFLAGS =

BUILD_DIR = build

SRC_FILES = $(wildcard src/*.c src/*/*.c)

build: $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(BUILD_DIR)/out

clean:
	rm $(BUILD_DIR)/*

# dirty the main file to force rebuild
dirty: src/main.c
	touch src/main.c
