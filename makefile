CC = gcc
CFLAGS = -Wall -Wextra -I./include
LDFLAGS = -lws2_32

SRC_DIR = src
BUILD_DIR = build
TARGET = server.exe

# Source files in src directory
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
# Add server.c to the list
ALL_SRCS = $(SRC_FILES) server.c

# Generate object file names
SRC_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))
SERVER_OBJ = $(BUILD_DIR)/server.o
ALL_OBJS = $(SRC_OBJS) $(SERVER_OBJ)

.PHONY: all clean

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files from src directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile server.c separately
$(SERVER_OBJ): server.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(ALL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)



# gcc -Wall -I./include -o server.exe server.c src/mime_types.c src/http_utils.c -lws2_32