CXX := mpic++
CFLAGS := -g -Wall -march=native

PROGRAM_NAME = nbody_simulation


SRC_DIRS := src
INC_DIRS :=
LIBS :=

DEFINE_FLAGS :=

GLAD_DIR = glad

ifdef enable_opengl
	SRC_DIRS += $(GLAD_DIR)/src
	INC_DIRS += $(GLAD_DIR)/include
	LIBS += -lGL -lglfw
	DEFINE_FLAGS += -DENABLE_OPENGL
endif




SRC := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c')
INC_FLAGS := $(addprefix -I, $(INC_DIRS))


.PHONY: all
all:
	$(CXX) $(CFLAGS) $(INC_FLAGS) $(SRC) -o $(PROGRAM_NAME) $(LIBS) $(DEFINE_FLAGS)