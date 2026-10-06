TARGET			:= sustain-editor
SRC_DIR 		:= src
INC_DIR     := inc
BUILD_DIR 	:= build
SRCS				:= $(shell find $(SRC_DIR) -name '*.c')
OBJS				:= $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS				:= $(OBJS:.o=.d)

CC					:= cc
STD         := -std=c99

# SDL3
SDL_DIR 		:= vendor/SDL
SDL_BUILD   := $(SDL_DIR)/build
SDL_LIB     := $(SDL_BUILD)/libSDL3.a


WARNINGS		:= -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
               -Wsign-conversion -Wcast-align -Wformat=2 \
               -Wundef -Wstrict-prototypes -Wmissing-prototypes

CXXFLAGS 		:= -D_POSIX_C_SOURCE=200809L -I$(INC_DIR) -I$(SDL_DIR)/include -MMD -MP
CFLAGS 			:= $(STD) $(WARNINGS)
LDFLAGS 		:= -L$(SDL_BUILD)
LDLIBS			:= -lSDL3 -lm -lpthread -ldl

MODE 				?= release

ifeq ($(MODE),debug)
    CFLAGS  += -O0 -g3 -DDEBUG
endif

ifeq ($(MODE),release)
	CFLAGS += -O2 -DNDEBUG
endif

ifeq ($(MODE),asan)
    CFLAGS  += -O0 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer
    LDFLAGS += -fsanitize=address,undefined
endif

.PHONY: all clean debug release asan run format lint help

all: $(TARGET)

$(SDL_LIB):
	cmake -S $(SDL_DIR) -B $(SDL_BUILD) -DSDL_STATIC=ON -DSDL_SHARED=OFF -DSDL_X11_XSCRNSAVER=OFF -DSDL_X11_XTEST=OFF
	cmake --build $(SDL_BUILD) --parallel

$(TARGET): $(SDL_LIB) $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

asan:
	$(MAKE) MODE=asan

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

format:
	clang-format -i $(SRC_DIR)/*.c

lint:
	clang-tidy $(SRCS) -- $(CXXFLAGS) $(CFLAGS)

help:
	@echo "Available targets:"
	@echo "  make            -> release build"
	@echo "  make debug      -> debug build with -O0 -g3"
	@echo "  make asan       -> build with AddressSanitizer + UBSan"
	@echo "  make run        -> build and run the binary"
	@echo "  make clean      -> remove build directory and binary"
	@echo "  make format     -> format sources with clang-format"
	@echo "  make lint       -> run static analysis with clang-tidy"

-include $(DEPS)

