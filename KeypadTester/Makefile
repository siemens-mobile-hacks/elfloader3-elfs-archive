PROJECT = keypad-tester
TARGETS := NSG ELKA

OPT := -Os
LDLIBS += -lgcc -lcrt -lcrt_helper
CPPFLAGS += -Wno-unused-variable
DEFINES += -D__NO_LIBC

SOURCES += main.c

SDK_PATH ?= ../../sdk
include $(SDK_PATH)/multi-target.mk
