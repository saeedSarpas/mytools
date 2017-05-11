CC := gcc
CFLAGS := -Wall -std=gnu11 -lconfig -lm -fopenmp

MAKE := make

MODULE_NAME := unmatched_halos

FILE := unmatched_halos.c

DEPS := ./../../../configfile/mylibconfig.c \
        ./../../../time/mytime.c \
        ./../../../memory/allocate.c \
        ./../../../memory/copy.c \
        ./../../../avltree/avltree.c \
        ./../../../avltree/avl_delete.c \
        ./../../../avltree/avl_insert.c \
        ./../../../avltree/avl_inorder_traversal.c \
        ./../../../io/open_file.c \
        ./../../../io/read_from.c \
        ./../../../io/write_to.c \
        ./../../../io/check_file.c \
        ./../../../io/check_dir.c \
        ./../../../vector/vector.c \
        ./../../../vector/vector_push.c \
        ./../../../strings/concat/concat.c \
        ./../../../halofinder/halofinder.c \
        ./../../../halofinder/rockstar/load_rockstar_bin.c \
        ./../../../simulation/snapshot.c \
        ./../../../simulation/check_if_saved_exist.c \
        ./../../../simulation/load_snap_from.c \
        ./../../../simulation/save_snap_to.c \
        ./../../../simulation/gadget/load_gadget_snap.c \
        ./../../../search/half_interval/find_ge.c \
        ./../../matchinghalos/load_mh.c \
        ./unmatched_halos_config.c

.PHONY : all

all :
	@$(CC) -o $(MODULE_NAME) $(FILE) $(DEPS) $(CFLAGS)
	@echo "[CC]\t$(CFLAGS) -o $(MODULE_NAME)"

clean :
	rm -f $(MODULE_NAME)
