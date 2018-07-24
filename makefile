CC = clang-6.0
CFLAGS =

BUILD_DIR = build

ROOT_FILES = ./lexer.c

SRC_FILES = $(ROOT_FILES)

build: $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(BUILD_DIR)/out

clean:
	rm $(BUILD_DIR)/*
