CC = gcc
CFLAGS = -Wall -std=gnu11

TESTRUNNER = cgreen-runner
TESTLIBS = -lcgreen

_FILES = load_rockstar_bin \
         ./../memory/allocate \
         ./../io/open_file \
         ./../io/read_from \
         sort_rockstar_halos
FILES = $(patsubst %,%.c,$(_FILES))
FILE_OBJS = $(patsubst %,%.o,$(_FILES))

_TESTS = load_rockstar_bin_test \
         sort_rockstar_halos_test
TESTS = $(patsubst %,%.c,$(_TESTS))
TEST_OBJS = $(patsubst %,%.o,$(_TESTS))

.PHONY : all test compile clean
all : test

$(_FILES):
	$(CC) $(CFLAGS) -fPIC -c $(FILES)

$(_TESTS):
	$(CC) $(CFLAGS) -fPIC -c $(TESTS)

compile : $(_FILES) $(_TESTS)
	cd ./../io; make compile
	cd ./../memory; make compile
	$(CC) $(CFLAGS) -shared -o halo_finder_tests.so $(FILE_OBJS) $(TEST_OBJS) $(TESTLIBS)

test : compile
	$(TESTRUNNER) halo_finder_tests.so

clean :
	@rm -f *.so *.o $(FILE_OBJS) $(TEST_OBJS)
