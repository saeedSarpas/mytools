MODULE_NAME := halomatcher

_FILES := matchinghalo \
          load_save_matches \
          singlehalo_matcher \
          halomatcher

_TESTS := matchinghalo_test \
          load_save_matches_test \
          singlehalo_matcher_test \
          halomatcher_test

_DEPS := ./../memory/allocate \
         ./../memory/copy \
         ./../search/half_interval/find_ge \
         ./../avltree/avl_tree \
         ./../vector/vector \
         ./../halofinder/halofinder \
         ./../loading/simple_loading \
         ./../io/open_file \
         ./../io/write_to \
         ./../io/read_from \
         ./../time/mytime

include ./../Makefile
