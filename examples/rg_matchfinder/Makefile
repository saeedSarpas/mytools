CC := gcc
CFLAGS := -Wall -std=gnu11 -lconfig -lm

MAKE := make

MODULE_NAME := rg_matchfinder

FILE := rg_matchfinder.c

DEPS := ./../../configfile/mylibconfig.c \
        ./../../time/mytime.c \
        ./../../halomatcher/load_save_matches.c \
        ./../../memory/allocate.c \
        ./../../memory/copy.c \
        ./../../io/open_file.c \
        ./../../io/read_from.c \
        ./../../io/write_to.c \
        ./../../io/check_file.c \
        ./../../strings/concat/concat.c \
        ./../../halofinder/halofinder.c \
        ./../../halofinder/rockstar/load_rockstar_bin.c \
        ./../../halofinder/rockstar/sort_rockstar_halos.c \
        ./../../halofinder/fill_init_volume.c \
        ./../../simulation/snapshot.c \
        ./../../simulation/load_snap_from.c \
        ./../../simulation/save_snap_to.c \
        ./../../simulation/check_if_saved_exist.c \
        ./../../simulation/gadget/load_gadget_snap.c \
        ./../../halomatcher/matchinghalo.c \
        ./../../halomatcher/singlehalo_matcher.c \
        ./../../halomatcher/halomatcher.c \
        ./../../avltree/avltree.c \
        ./../../avltree/avl_find.c \
        ./../../avltree/avl_insert.c \
        ./../../avltree/avl_delete.c \
        ./../../avltree/avl_inorder_traversal.c \
        ./../../grid/row_major_order/point_to_grid.c \
        ./../../grid/row_major_order/three_to_one.c \
        ./../../search/half_interval/find_ge.c \
        ./../../loading/simple_loading.c \
        ./../../vector/vector.c \
        ./../../vector/vector_push.c \
        ./../../vector/vector_get.c \
        ./../../math/interval/expspaced.c \
        ./../../sort/mergesort/mergesort.c

.PHONY : all

all :
	@$(CC) -o $(MODULE_NAME) $(FILE) $(DEPS) $(CFLAGS)
	@echo "[CC]\t$(CFLAGS) -o $(MODULE_NAME)"

clean :
	rm -f $(MODULE_NAME)
