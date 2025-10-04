CC = clang
CFLAGS = -g -Wall -Werror -Wextra

SRC_DIR = src
BUILD_DIR = .build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.d)

TARGET = ./$(BUILD_DIR)/cql

.PHONY: all clean

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) -r $(OBJS) $(DEPS) $(TARGET)