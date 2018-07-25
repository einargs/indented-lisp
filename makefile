CC = clang-6.0
CFLAGS =

BUILD_DIR = build

SRC_FILES = src/main.c src/*/*.c

build: $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(BUILD_DIR)/out

clean:
	rm $(BUILD_DIR)/*
