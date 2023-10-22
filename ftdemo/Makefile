PROJECT = ftdemo

OPT := -Os
LDLIBS += -lcrt -lcrt_helper -lgcc -lm-1.0 -lc-1.0 -lstdc++-1.2.2 -lsupc++-1.2.2 -lft-2.4.6-1 -lft_server

SOURCES += config_data.c
SOURCES += main.cpp conf_loader.cpp ftdemo.cpp
SOURCES += ALIB/font.cpp ALIB/freetype.cpp ALIB/img.cpp ALIB/io.cpp ALIB/ipc.cpp ALIB/key.cpp 

SDK_PATH ?= ../sdk
include $(SDK_PATH)/multi-target.mk
