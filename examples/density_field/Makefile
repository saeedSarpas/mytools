CC := gcc
CFLAGS := -Wall -std=gnu11 -lconfig -lm

MAKE := make

MODULE_NAME := density_field

FILE := density_field.c

DEPS := ./../../memory/allocate.c \
        ./../../io/open_file.c \
        ./../../io/read_from.c \
        ./../../io/write_to.c \
        ./../../io/check_file.c \
        ./../../simulation/snapshot.c \
        ./../../simulation/gadget/load_gadget_snap.c \
        ./../../simulation/load_snap_from.c \
        ./../../simulation/save_snap_to.c \
        ./../../time/mytime.c \
        ./../../massassignmentfunctions/cic/cic.c \
        ./../../grid/row_major_order/three_to_one.c \
        ./../../strings/concat/concat.c \
        ./../../loading/simple_loading.c

.PHONY : all

all :
	@$(CC) -o $(MODULE_NAME) $(FILE) $(DEPS) $(CFLAGS)
	@echo "[CC]\t$(CFLAGS) -o $(MODULE_NAME)"

clean :
	rm -f $(MODULE_NAME)
