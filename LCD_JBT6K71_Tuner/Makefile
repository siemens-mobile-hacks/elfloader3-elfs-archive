PROJECT = LCD_JBT6K71_Tuner
TARGETS := ELKA

OPT := -Os
LDLIBS += -lgcc -lcrt -lcrt_helper
CPPFLAGS += -Wno-unused-variable
DEFINES += -D__NO_LIBC

SOURCES += main.c

SDK_PATH ?= ../../sdk
include $(SDK_PATH)/multi-target.mk
