CXX := mpic++
CFLAGS := -std=c++17 -g -Wall -march=native

PROGRAM_NAME = nbody_simulation


SRC_DIRS := src
INC_DIRS :=
LIBS :=

DEFINE_FLAGS :=

glad_dir = glad

ifeq ($(enable_opengl), 1)
	SRC_DIRS += $(glad_dir)/src
	INC_DIRS += $(glad_dir)/include
	LIBS += -lGL -lglfw
	DEFINE_FLAGS += -DENABLE_OPENGL
endif


SRC := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c')
INC_FLAGS := $(addprefix -I, $(INC_DIRS))


.PHONY: all
all:
	$(CXX) $(CFLAGS) $(INC_FLAGS) $(SRC) -o $(PROGRAM_NAME) $(LIBS) $(DEFINE_FLAGS)