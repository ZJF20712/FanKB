XM04_DIR = xm04

SRC +=  $(XM04_DIR)/suart.S \
	$(XM04_DIR)/xm04.c \
	$(XM04_DIR)/xm04_task.c \
	$(XM04_DIR)/main.c

OPT_DEFS += -DPROTOCOL_XM04

VPATH += $(XM04_DIR)
