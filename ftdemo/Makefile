PROJECT = ftdemo

OPT := -Os
LDLIBS += -lcrt -lcrt_helper -lgcc -lm -lc -lc++ -lsupc++ -lft -lft_server

SOURCES += config_data.c
SOURCES += main.cpp conf_loader.cpp ftdemo.cpp
SOURCES += ALIB/font.cpp ALIB/freetype.cpp ALIB/img.cpp ALIB/io.cpp ALIB/ipc.cpp ALIB/key.cpp 

SDK_PATH ?= ../sdk
include $(SDK_PATH)/multi-target.mk
