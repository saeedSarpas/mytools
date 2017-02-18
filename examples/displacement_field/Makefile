CC := gcc
CFLAGS := -Wall -std=gnu11 -lconfig -lm

MAKE := make

MODULE_NAME := displacement_field

FILE := displacement_field.c

DEPS := ./../../configfile/mylibconfig.c \
        ./../../time/mytime.c \
        ./../../memory/allocate.c \
        ./../../memory/copy.c \
        ./../../io/open_file.c \
        ./../../io/read_from.c \
        ./../../io/write_to.c \
        ./../../io/check_file.c \
        ./../../strings/concat/concat.c \
        ./../../simulation/snapshot.c \
        ./../../simulation/load_snap_from.c \
        ./../../simulation/save_snap_to.c \
        ./../../simulation/gadget/load_gadget_snap.c \
        ./../../grid/row_major_order/three_to_one.c

.PHONY : all

all :
	@$(CC) -o $(MODULE_NAME) $(FILE) $(DEPS) $(CFLAGS)
	@echo "[CC]\t$(CFLAGS) -o $(MODULE_NAME)"

clean :
	rm -f $(MODULE_NAME)
