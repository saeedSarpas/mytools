CC = gcc
CFLAGS = -Wall -std=gnu11

TESTRUNNER = cgreen-runner
TESTLIBS = -lcgreen

_FILES = avl_tree \
         ./../memory/allocate
FILES = $(patsubst %,%.c,$(_FILES))
FILE_OBJS = $(patsubst %,%.o,$(_FILES))

_TESTS = avl_tree_test
TESTS = $(patsubst %,%.c,$(_TESTS))
TEST_OBJS = $(patsubst %,%.o,$(_TESTS))

.PHONY : all test compile clean
all : test

$(_FILES):
	$(CC) $(CFLAGS) -fPIC -c $(FILES)

$(_TESTS):
	$(CC) $(CFLAGS) -fPIC -c $(TESTS)

compile : $(_FILES) $(_TESTS)
	make -C ./../io compile
	$(CC) $(CFLAGS) -shared -o avl_tree_tests.so $(FILE_OBJS) $(TEST_OBJS) $(TESTLIBS)

test : compile
	$(TESTRUNNER) avl_tree_tests.so

clean :
	@rm -f *.so *.o $(FILE_OBJS) $(TEST_OBJS)
