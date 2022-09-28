CC = gcc
CFLAGS = -D DEBUG --std=gnu17 -Wall -Wextra -ggdb 

SRC_DIR = ..
TEST_DIR = .

ROOT_DIR = ../..
COMMON_DIR = $(ROOT_DIR)/common
3RDP_DIR = $(ROOT_DIR)/3rdp

IDIRS = -I$(SRC_DIR) -I$(COMMON_DIR) -I$(3RDP_DIR)
CFLAGS += $(IDIRS)

LIBS = -lczmq

BUILD_DIR = .

default: tests
all: tests
.PHONY: clean

HEADERS = $(wildcard $(SRC_DIR)/*.h)
HEADERS += $(wildcard $(COMMON_DIR)/*.h)

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
OBJS += $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(TEST_DIR)/*.c))
OBJS += $(BUILD_DIR)/munit.o

# remove real main to use test main
TMPOBJS := $(OBJS)
OBJS = $(filter-out $(BUILD_DIR)/server_main.o, $(TMPOBJS));

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)	

$(BUILD_DIR)/munit.o: $(3RDP_DIR)/munit/munit.c
	$(CC) -c -o $@ $< $(CFLAGS)	

tests: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

valgrind: tests
	valgrind -q --leak-check=full --show-leak-kinds=all  ./tests

xsan: CFLAGS += -fsanitize=address,undefined # -O1 -fno-omit-frame-pointer
xsan: clean tests
	./tests && rm -f $(BUILD_DIR)/*.o tests

clean:
	rm -f $(BUILD_DIR)/*.o tests
