CC             ?= clang

COMMON_CFLAGS  := -I.
COMMON_CFLAGS  += -Imodules/foundation
COMMON_CFLAGS  += -MMD
COMMON_CFLAGS  += -Wall
COMMON_CFLAGS  += -Wextra
COMMON_CFLAGS  += -Wconversion
COMMON_CFLAGS  += -Wno-sign-conversion
COMMON_CFLAGS  += -O3
COMMON_CFLAGS  += -fno-stack-protector
COMMON_CFLAGS  += -fno-builtin

FOUNDATION_BIN := modules/foundation/bes-foundation.a

EGG_BIN        := egg.a
EGG_SRC_DIR    := bes/egg
EGG_OBJ_DIR    := obj/egg
EGG_SRC_FILES  := $(wildcard $(EGG_SRC_DIR)/*.c)
EGG_OBJ_FILES  := $(patsubst $(EGG_SRC_DIR)/%.c,$(EGG_OBJ_DIR)/%.o,$(EGG_SRC_FILES))
EGG_DEP_FILES  := $(EGG_OBJ_FILES:.o=.d)
EGG_CFLAGS     := $(COMMON_CFLAGS)
EGG_CFLAGS     += -fPIC

REPL_BIN       := egg
REPL_SRC_DIR   := src
REPL_OBJ_DIR   := obj
REPL_SRC_FILES := $(wildcard $(REPL_SRC_DIR)/*.c)
REPL_OBJ_FILES := $(patsubst $(REPL_SRC_DIR)/%.c,$(REPL_OBJ_DIR)/%.o,$(REPL_SRC_FILES))
REPL_DEP_FILES := $(REPL_OBJ_FILES:.o=.d)
REPL_CFLAGS    := $(COMMON_CFLAGS)

all: $(REPL_BIN)

$(FOUNDATION_BIN):
	$(MAKE) -C $(dir $@) $(notdir $@)

$(EGG_OBJ_DIR)/%.o: $(EGG_SRC_DIR)/%.c
	$(CC) $(EGG_CFLAGS) -c -o $@ $<

$(REPL_OBJ_DIR)/%.o: $(REPL_SRC_DIR)/%.c
	$(CC) $(REPL_CFLAGS) -c -o $@ $<

$(EGG_BIN): $(EGG_OBJ_FILES)
	$(AR) -r -o $@ $^

$(REPL_BIN): $(REPL_OBJ_FILES) $(EGG_BIN) $(FOUNDATION_BIN)
	$(CC) -o $@ $^

clean:
	rm -rf $(EGG_BIN) $(EGG_OBJ_FILES) $(EGG_DEP_FILES)
	rm -rf $(REPL_BIN) $(REPL_OBJ_FILES) $(REPL_DEP_FILES)
	$(MAKE) -C $(dir $(FOUNDATION_BIN)) clean

.PHONY: clean $(FOUNDATION_BIN)

.NOTPARALLEL: $(FOUNDATION_BIN)
