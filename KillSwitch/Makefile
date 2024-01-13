PROJECT = killswitch
TARGETS := NSG ELKA

OPT := -Os
LDLIBS += -lgcc -lcrt -lcrt_helper
CPPFLAGS += -Wno-unused-variable
DEFINES += -D__NO_LIBC

SOURCES += main.c stopwatch.c conf_loader.c conf_data.c

SDK_PATH ?= ../../sdk
include $(SDK_PATH)/multi-target.mk
