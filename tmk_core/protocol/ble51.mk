BLE51_DIR = protocol/ble51

SRC +=  serial_uart.c \
        $(BLE51_DIR)/ble51.c \
        $(BLE51_DIR)/ble51_task.c \
        $(BLE51_DIR)/main.c

OPT_DEFS += -DPROTOCOL_BLE51

VPATH += $(TMK_DIR)/$(BLE51_DIR)
