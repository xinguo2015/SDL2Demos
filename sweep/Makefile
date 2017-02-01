#
# 刘新国 
#
NAME  := sweep
BUILD := build/#
SRC   := src/#
CC    := gcc
CXX   := g++
ver = debug#
ver = release#
ifeq ($(ver), debug)
	TARGET  := $(NAME)-d#
	OBJDir  := $(BUILD)debug/#
	CCFLAGS := -g -Wall -D_debug
else
	TARGET  := $(NAME)#
	OBJDir  := $(BUILD)release/#
	CCFLAGS := -o3 -Wall
endif

# SDL libs
LIBS := -lm -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lSDL2_mixer
# SDL header
INCS := -I/usr/include/SDL2 

TARGET  := $(BUILD)$(TARGET)#
CFILE   := $(wildcard $(SRC)*.c)
OBJS    := $(patsubst %.c, %.o, $(CFILE))
DEPS    := $(patsubst %.o, %.d, $(OBJS))
OBJS    := $(addprefix $(OBJDir), $(OBJS))
DEPS    := $(addprefix $(OBJDir), $(DEPS))

$(TARGET) : $(OBJS) $(DEPS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

.PHONY : test cleanobj cleandep clean

cleanobj :
	rm -f $(OBJS)
cleandep :
	rm -f $(DEPS)
clean:
	rm -rf $(BUILD)

# Compile .c
$(OBJDir)%.o: %.c
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	$(CC) -c $(CCFLAGS) $(INCS) -o $@ $<
# Compile .cpp
$(OBJDir)%.o: %.cpp
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	$(CC) -c $(CCFLAGS) $(INCS) -o $@ $<
# c file dependence
$(OBJDir)%.d: %.c
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	gcc -MM -MT '$(patsubst %.c, $(OBJDir)%.o, $<) $@' $< > $@
# cpp file dependence
$(OBJDir)%.d: %.cpp
	@set -e; if ! test -d $(@D); then mkdir -p $(@D); fi
	gcc -MM -MT '$(patsubst %.cpp, $(OBJDir)%.o, $<) $@' $< > $@

test:
	echo $(BUILD)$(BUILD)
	echo $(ROOT)

-include $(DEPS)

