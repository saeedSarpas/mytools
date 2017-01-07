CC := gcc
CFLAGS := -Wall -std=gnu11 -lconfig -lm

MAKE := make

MODULE_NAME := triplematches

FILE := triplematches.c

DEPS := ./../../../configfile/mylibconfig.c \
        ./../../../time/mytime.c \
        ./../../../memory/allocate.c \
        ./../../../memory/copy.c \
        ./../../../avltree/avltree.c \
        ./../../../avltree/avl_find.c \
        ./../../../avltree/avl_delete.c \
        ./../../../avltree/avl_insert.c \
        ./../../../vector/vector.c \
        ./../../../vector/vector_get.c \
        ./../../../vector/vector_push.c \
        ./../../../strings/concat/concat.c \
        ./../../../io/open_file.c \
        ./../../../io/read_from.c \
        ./../../../io/write_to.c \
        ./../../../io/check_file.c \
        ./../../../io/check_dir.c \
        ./../../../ll/ll.c \
        ./../../../ll/ll_add.c \
        ./../../../ll/ll_traverse.c \
        ./../../../halofinder/halofinder.c \
        ./../../../halofinder/rockstar/load_rockstar_bin.c \
        ./../../../simulation/snapshot.c \
        ./../../../simulation/gadget/load_gadget_snap.c \
        ./../../../simulation/load_snap_from.c \
        ./../../../simulation/save_snap_to.c \
        ./../../../simulation/check_if_saved_exist.c \
        ./../../matchinghalos/load_mh.c \
        ./../triplecascade/triplecascade.c \
        ./../mh_cascade/mh_cascade.c

.PHONY : all

all :
	@$(CC) -o $(MODULE_NAME) $(FILE) $(DEPS) $(CFLAGS)
	@echo "[CC]\t$(CFLAGS) -o $(MODULE_NAME)"

clean :
	rm -f $(MODULE_NAME)
