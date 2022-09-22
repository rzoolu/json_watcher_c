CC = gcc
CFLAGS = -D DEBUG --std=gnu17 -Wall -Wextra -ggdb 

SRC_DIR = server_src
COMMON_DIR = common
3RDP_DIR = 3rdp

IDIRS = -I$(SRC_DIR) -I$(COMMON_DIR) -I$(3RDP_DIR)
CFLAGS += $(IDIRS)

BUILD_DIR = build

LDIR =
LIBS =

default: server
all: server client
.PHONY: clean

# server
HEADERS = $(wildcard $(SRC_DIR)/*.h)
HEADERS += $(wildcard $(COMMON_DIR)/*.h)

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

xsan_server: CFLAGS += -fsanitize=address,undefined # -O1 -fno-omit-frame-pointer
xsan_server: server

# client
CLIENT_SRC_DIR=client_src

CLIENT_HEADERS = $(wildcard $(CLIENT_SRC_DIR)/*.h)
CLIENT_HEADERS += $(wildcard $(COMMON_DIR)/*.h)

CLIENT_OBJS = $(patsubst $(CLIENT_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(CLIENT_SRC_DIR)/*.c))

$(BUILD_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c $(CLIENT_HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(CLIENT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

xsan_client: CFLAGS += -fsanitize=address,undefined # -O1 -fno-omit-frame-pointer
xsan_client: client

clean:
	rm -f $(BUILD_DIR)/*.o server client