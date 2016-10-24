CC := gcc
CFLAGS := -Wall -std=gnu11 -lm

MAKE := make compile

TESTRUNNER := cgreen-runner
TESTLIBS := -lcgreen

FILES := $(addsuffix .c,$(_FILES))
FILE_OBJS := $(addsuffix .o,$(_FILES))

TESTS := $(addsuffix .c,$(_TESTS))
TEST_OBJS := $(addsuffix .o,$(_TESTS))

DEPDIRS := $(dir $(_DEPS))
DEP_OBJS := $(addsuffix .o,$(_DEPS))

.PHONY : all test compile clean
all : test

$(FILE_OBJS) : $(FILES)
	@$(CC) -fPIC -c $(FILES) $(CFLAGS)
	@echo "[CC]\t-fPIC -c $(FILES) $(CFLAGS)"

$(TEST_OBJS) : $(TESTS) $(FILE_OBJS)
	@$(CC) -fPIC -c $(TESTS) $(CFLAGS)
	@echo "[CC]\t-fPIC -c $(TESTS) $(CFLAGS)"

define dep_compile
@echo "[MAKE]\t--directory $(1)"; $(MAKE) --directory $(1) $(\n)
endef

dep_compile :
	$(foreach dir, $(DEPDIRS), $(call dep_compile, $(dir)))

compile : $(FILE_OBJS) $(TEST_OBJS) dep_compile

$(MODULE_NAME)_tests.so : compile
	@$(CC) -shared -o $(MODULE_NAME)_tests.so $(FILE_OBJS) $(TEST_OBJS) $(DEP_OBJS) $(TESTLIBS) $(CFLAGS)
	@echo "[CC]\t-shared $(CFLAGS) -o $(MODULE_NAME)_tests.so"

test : $(MODULE_NAME)_tests.so
	@echo ""
	@echo "[TESTRUNNER]\t$(MODULE_NAME)_tests.so"
	@echo "-----------------------------------------------------------------------"
	@$(TESTRUNNER) $(MODULE_NAME)_tests.so
	@echo "-----------------------------------------------------------------------"

clean :
	rm -f $(MODULE_NAME)_tests.so $(FILE_OBJS) $(TEST_OBJS) $(DEP_OBJS)

define \n


endef