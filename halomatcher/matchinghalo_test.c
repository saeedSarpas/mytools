/*
 * halomatcher/matchinghalo_test.c
 * test_for: halomatcher/matchinghalo.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "matchinghalo.h"
#include "./../memory/allocate.h"


#define NUMHALOS 11


Describe(matchinghalo);
BeforeEach(matchinghalo){};
AfterEach(matchinghalo){};


Ensure(matchinghalo, creates_and_dispose_matchinghalo_struct_properly)
{
  matchinghalo *mh = new_matchinghalo(NUMHALOS);
  assert_that(mh, is_non_null);
  assert_that(mh->len, is_equal_to(NUMHALOS));
  assert_that(mh->matchingids, is_non_null);
  assert_that(mh->goodnesses, is_non_null);

  int i;
  for(i = 0; i < mh->len; i++){
    assert_that(mh->matchingids[i], is_equal_to(MATCHINGHALONOTSET));
    assert_that_double(mh->goodnesses[i], is_equal_to_double(0.0));
  }

  mh->dispose(mh);
  assert_that(mh->matchingids, is_null);
  assert_that(mh->goodnesses, is_null);
}
