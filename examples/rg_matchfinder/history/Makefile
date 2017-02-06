CC := gcc
CFLAGS := -Wall -std=gnu11 -lconfig -lm

MAKE := make

MODULE_NAME := history

FILE := history.c

DEPS := ./../../../configfile/mylibconfig.c \
        ./../../../time/mytime.c \
        ./../../../memory/allocate.c \
        ./../../../memory/copy.c \
        ./../../../avltree/avltree.c \
        ./../../../avltree/avl_delete.c \
        ./../../../avltree/avl_insert.c \
        ./../../../avltree/avl_find.c \
        ./../../../vector/vector.c \
        ./../../../vector/vector_get.c \
        ./../../../vector/vector_push.c \
        ./../../../strings/concat/concat.c \
        ./../../../io/open_file.c \
        ./../../../io/read_from.c \
        ./../../../io/check_file.c \
        ./../../../io/check_dir.c \
        ./../../../ll/ll.c \
        ./../../../ll/ll_add.c \
        ./../../../ll/ll_traverse.c \
        ./../../../halofinder/halofinder.c \
        ./../../../halofinder/rockstar/load_rockstar_bin.c \
        ./../../../math/fittingfunctions/fityax.c \
        ./../../../loading/simple_loading.c \
        ./../../matchinghalos/load_mh.c \
        ./../triplecascade/triplecascade.c \
        ./../mh_cascade/mh_cascade.c \
        ./history_config.c \
        ./accretion_history.c

.PHONY : all

all :
	@$(CC) -o $(MODULE_NAME) $(FILE) $(DEPS) $(CFLAGS)
	@echo "[CC]\t$(CFLAGS) -o $(MODULE_NAME)"

clean :
	rm -f $(MODULE_NAME)
