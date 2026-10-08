NAME        := sustain
LIB_STATIC  := lib$(NAME).a
LIB_SHARED  := lib$(NAME).so

SRC_DIR     := src
INC_DIR     := inc
BUILD_DIR   := build

SRCS        := $(shell find $(SRC_DIR) -name '*.c')

CC          := cc
AR          := ar
STD         := -std=c99

# SDL3
SDL_DIR     := vendor/SDL
SDL_BUILD   := $(SDL_DIR)/build
SDL_LIB     := $(SDL_BUILD)/libSDL3.a

# cglm
CGLM_DIR    := vendor/cglm
CGLM_BUILD  := $(CGLM_DIR)/build
CGLM_LIB    := $(CGLM_BUILD)/libcglm.a

WARNINGS    := -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
               -Wsign-conversion -Wcast-align -Wformat=2 \
               -Wundef -Wstrict-prototypes -Wmissing-prototypes

CPPFLAGS    := -D_POSIX_C_SOURCE=200809L -I$(INC_DIR) -I$(SDL_DIR)/include \
               -I$(CGLM_DIR)/include -MMD -MP
CFLAGS      := $(STD) $(WARNINGS)
SO_LDFLAGS  := -shared -Wl,-soname,$(LIB_SHARED)

MODE        ?= release

ifeq ($(MODE),debug)
    CFLAGS     += -O0 -g3 -DDEBUG
endif

ifeq ($(MODE),release)
    CFLAGS     += -O2 -DNDEBUG
endif

ifeq ($(MODE),asan)
    CFLAGS     += -O0 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer
    SO_LDFLAGS += -fsanitize=address,undefined
endif

OBJ_S       := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/$(MODE)/static/%.o,$(SRCS))
OBJ_D       := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/$(MODE)/shared/%.o,$(SRCS))
DEPS        := $(OBJ_S:.o=.d) $(OBJ_D:.o=.d)

.PHONY: all static shared deps clean debug release asan format lint help

all: static shared

static: $(LIB_STATIC)
shared: $(LIB_SHARED)

$(LIB_STATIC): $(OBJ_S)
	$(AR) rcs $@ $^

$(LIB_SHARED): $(OBJ_D) $(SDL_LIB) $(CGLM_LIB)
	$(CC) $(SO_LDFLAGS) $(OBJ_D) -o $@ \
	      -L$(SDL_BUILD) -L$(CGLM_BUILD) -lSDL3 -lcglm -lm -lpthread -ldl

$(BUILD_DIR)/$(MODE)/static/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(MODE)/shared/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

deps: $(SDL_LIB) $(CGLM_LIB)

$(SDL_LIB):
	cmake -S $(SDL_DIR) -B $(SDL_BUILD) -DSDL_STATIC=ON -DSDL_SHARED=OFF \
	      -DSDL_X11_XSCRNSAVER=OFF -DSDL_X11_XTEST=OFF \
	      -DCMAKE_POSITION_INDEPENDENT_CODE=ON
	cmake --build $(SDL_BUILD) --parallel

$(CGLM_LIB):
	meson setup $(CGLM_BUILD) $(CGLM_DIR) -Ddefault_library=static
	ninja -C $(CGLM_BUILD)

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

asan:
	$(MAKE) MODE=asan

clean:
	rm -rf $(BUILD_DIR) $(LIB_STATIC) $(LIB_SHARED)

format:
	clang-format -i $(shell find $(SRC_DIR) $(INC_DIR) -name '*.c' -o -name '*.h')

lint:
	clang-tidy $(SRCS) -- $(CPPFLAGS) $(CFLAGS)

help:
	@echo "Available targets:"
	@echo "  make          -> libsustain.a + libsustain.so (release)"
	@echo "  make static   -> build libsustain.a only"
	@echo "  make shared   -> build libsustain.so only"
	@echo "  make deps     -> build SDL3 and cglm"
	@echo "  make debug    -> debug build with -O0 -g3"
	@echo "  make asan     -> build with AddressSanitizer + UBSan"
	@echo "  make clean    -> remove build directory and libraries"
	@echo "  make format   -> format sources with clang-format"
	@echo "  make lint     -> run static analysis with clang-tidy"

-include $(DEPS)
