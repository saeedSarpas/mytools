/*
 * time/mytime_test.c
 * test_for: time/mytime.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "mytime.h"


Describe(mytime);
BeforeEach(mytime) {}
AfterEach(mytime) {}


Ensure(mytime, works_properly) {}
