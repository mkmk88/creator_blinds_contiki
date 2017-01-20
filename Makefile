TARGET = mikro-e
CONTIKI=LetMeCreateIoT/contiki
CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL = 0
USE_CA8210 = 1
LWM2M_DIR=LetMeCreateIoT/AwaLWM2M
VERSION? = $(shell git describe --abbrev=4 --dirty --always --tags)

CFLAGS += -DVERSION=$(VERSION) -DPROJECT_CONF_H=\"project-conf.h\"
CFLAGS += -Wall -Wno-pointer-sign -g
CFLAGS += -I $(CONTIKI)/platform/$(TARGET)
CFLAGS += -I$(LWM2M_DIR)/api/include -DLWM2M_CLIENT
CFLAGS += -DUIP_CONF_BUFFER_SIZE=4096
CFLAGS += -fno-short-double
LDFLAGS += -Wl,--defsym,_min_heap_size=32000

SMALL=0

APPDIRS += $(LWM2M_DIR)/core/src
APPS += erbium
APPS += client
APPS += common

PROJECT_SOURCEFILES += blinds_debug.c

all: main
	xc32-bin2hex main.$(TARGET)
	cp main.mikro-e main.elf

include $(CONTIKI)/Makefile.include

clean:
	rm -rf contiki-mikro-e.a contiki-mikro-e.map main.elf main.hex main.mikro-e obj_mikro-e symbols.c symbols.h
