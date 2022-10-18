PROJECT = test_liquid

BUILD_DIR = build
SOURCE_DIR = Core/Src

C_INCLUDES = \
-ICore/Include \
-Ithirdparty/liquid-dsp/include

C_DEFS =
CFLAGS = $(C_DEFS) $(C_INCLUDES) -Wall -g -Wno-deprecated-declarations
#Dependency information
CFLAGS+= -MMD -MP -MF"$(@:%.o=%.d)"

LIBS = -lliquid -lc -lm 
LIBDIR = -Lthirdparty/liquid-dsp/
LDFLAGS = $(LIBDIR) $(LIBS)

SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: $(PROJECT).bin

run: $(PROJECT).bin 
	@(cd res; python3 generate.py)
	@./$<
	@(cd res; python3 visualize.py processed.txt)

debug: $(PROJECT).bin 
	@gdb ./$<

$(PROJECT).bin: $(OBJECTS) 
	@rm -f ./thirdparty/liquid-dsp/*.so
	@$(CC) $^ $(LDFLAGS) -o $@
	$(info [GENERATING] $@)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(BUILD_DIR)
	@$(CC) -c $(CFLAGS) $< -o $@
	$(info [COMPILING] $<)

$(BUILD_DIR):
	@mkdir $@

clean:
	@rm -fR $(BUILD_DIR)
	@rm -f $(PROJECT).bin
	@echo Project cleaned!
