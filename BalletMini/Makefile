PROJECT = BalletMini

OPT := -Os
LDLIBS += -lcrt -lcrt_helper -lgcc -lc -ljpeg -lpng -lz -lm
CFLAGS += -mthumb -Wno-missing-prototypes -Wno-redundant-decls -Wno-sign-compare -Wno-implicit-fallthrough

SOURCES += main.c
SOURCES += additems.c
SOURCES += config_data.c
SOURCES += conf_loader.c
SOURCES += destructors.c
SOURCES += display_utils.c
SOURCES += fileman.c
SOURCES += file_works.c
SOURCES += history.c
SOURCES += inet.c
SOURCES += lang.c
SOURCES += mainmenu.c
SOURCES += parse_oms.c
SOURCES += rand.c
SOURCES += readimg.c
SOURCES += string_works.c
SOURCES += upload.c
SOURCES += urlstack.c
SOURCES += url_utils.c

SDK_PATH ?= ../sdk
include $(SDK_PATH)/multi-target.mk
