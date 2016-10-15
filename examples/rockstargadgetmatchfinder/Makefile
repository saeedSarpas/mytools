CC := gcc
CFLAGS := -Wall -std=gnu11 -lconfig

MAKE := make

MODULE_NAME := rockstargadgetmatchfinder

FILE := rockstargadgetmatchfinder.c

DEPS := ./../../configfile/mylibconfig.c \
        ./../../memory/allocate.c \
        ./../../memory/copy.c \
        ./../../io/open_file.c \
        ./../../io/read_from.c \
        ./../../halofinder/halofinder.c \
        ./../../halofinder/rockstar/load_rockstar_bin.c \
        ./../../halofinder/rockstar/sort_rockstar_halos.c \
        ./../../halofinder/fill_init_volume.c \
        ./../../simulation/snapshot.c \
        ./../../simulation/gadget/load_gadget_snap.c \
        ./../../halomatcher/matchinghalo.c \
        ./../../halomatcher/halomatcher.c \
        ./../../avltree/avl_tree.c \
        ./../../grid/row_major_order/point_to_grid.c \
        ./../../grid/row_major_order/three_to_one.c \
        ./../../search/half_interval/find_ge.c \
        ./../../vector/vector.c \
        ./../../loading/simple_loading.c

.PHONY : all

all :
	@$(CC) -o $(MODULE_NAME).out $(FILE) $(DEPS) $(CFLAGS)
	@echo "[CC]\t$(CFLAGS) -o $(MODULE_NAME).out"

clean :
	rm -f $(MODULE_NAME).out
