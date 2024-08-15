CXX := mpic++
CFLAGS := -std=c++17 -g -Wall -march=native

PROGRAM_NAME = nbody_simulation


SRC_DIRS := src
INC_DIRS :=
LIB_DIRS :=
LIBS :=

DEFINE_FLAGS :=

glad_dir = glad

ifeq ($(enable_opengl), 1)
	SRC_DIRS += $(glad_dir)/src
	INC_DIRS += $(glad_dir)/include
	INC_DIRS += $(glfw_inc_dir)
	LIB_DIRS += $(glfw_lib_dir)
	LIBS += -lGL -lglfw
	DEFINE_FLAGS += -DENABLE_OPENGL
endif


SRC := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c')
INC_FLAGS := $(addprefix -I, $(INC_DIRS))
LIB_FLAGS := $(addprefix -L, $(LIB_DIRS))


.PHONY: all
all:
	$(CXX) $(CFLAGS) $(INC_FLAGS) $(LIB_FLAGS) $(SRC) -o $(PROGRAM_NAME) $(LIBS) $(DEFINE_FLAGS)