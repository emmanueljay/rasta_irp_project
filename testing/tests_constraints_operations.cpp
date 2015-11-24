// This sample shows how to write a simple unit test for a function,
// using Google C++ testing framework.
//
// Writing a unit test using Google C++ testing framework is easy as 1-2-3:


// Step 1. Include necessary header files such that the stuff your
// test logic needs is declared.
//
// Don't forget gtest.h, which declares the testing framework.

#include "bo/context.h"
#include "bo/tags.h"
#include "utils/helpers.h"

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <iostream>


// Step 2. Use the TEST macro to define your tests.
//
// TEST has two parameters: the test case name and the test name.
// After using the macro, you should define your test logic between a
// pair of braces.  You can use a bunch of macros to indicate the
// success or failure of a test.  EXPECT_TRUE and EXPECT_EQ are
// examples of such macros.  For a complete list, see gtest.h.
//
// <TechnicalDetails>
//
// In Google Test, tests are grouped into test cases.  This is how we
// keep test code organized.  You should put logically related tests
// into the same test case.
//
// The test case name and the test name should both be valid C++
// identifiers.  And you should not use underscore (_) in the names.
//
// Google Test guarantees that each test you define is run exactly
// once, but it makes no guarantee on the order the tests are
// executed.  Therefore, you should write your tests in such a way
// that their results don't depend on their order.
// </TechnicalDetails>


// 
/** SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT
 * One shift, 2 operations, to close to each other
 */
TEST(Operations_Constraints_Actives, SHI02_OP) {
  // Construction of Context
  google::InitGoogleLogging("test_constraints");
  Context context("../instances/Instance_V_1.1.xml");

  // Construction of Solution for SHI02
  std::vector<Shift>* shifts_l = context.solution()->shifts();
  Driver const& first_driver_l = context.data()->drivers()->begin()->second;
  int starting_time_l = first_driver_l.timeWindows(0).first;
  int setup_time; 
  int point_index;

  shifts_l->emplace_back(
    Shift(
      shifts_l->size(), // index
      first_driver_l.index(), // driver
      first_driver_l.trailer(), // trailer
      starting_time_l)); // Start

  point_index = 2;
  setup_time = rip::helpers::setup_time(point_index,*(context.data()));
  shifts_l->at(0).operations_ptr()->emplace_back(
    Operation(
      point_index, // point
      starting_time_l, // arrival
      264, // quantity
      setup_time // setup_time
      ));

  point_index = 4;
  setup_time = rip::helpers::setup_time(point_index,*(context.data()));
  shifts_l->at(0).operations_ptr()->emplace_back(
    Operation(
      point_index, // point
      starting_time_l + 12, // arrival 12 < timeMatrices(2,4)
      264, // quantity
      setup_time // setup_time
      ));

  // Test 
  int shift_l,operation_l;
  EXPECT_EQ(rip::tags::get_string(context.solution()->is_admissible(&shift_l,&operation_l)),
    rip::tags::get_string(SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT));
}


/** SHI03_LOADING_AND_DELIVERY_OPERATIONS_TAKE_A_CONSTANT_TIME
 * One shift, 1 operation, with departure(o) != arrival(o) + SetupTime(point(o))
 */
TEST(Operations_Constraints_Actives, SHI03) {
  // Construction of Context
  Context context("../instances/Instance_V_1.1.xml");

  // Construction of Solution for SHI02
  std::vector<Shift>* shifts_l = context.solution()->shifts();
  Driver const& first_driver_l = context.data()->drivers()->begin()->second;
  int starting_time_l = first_driver_l.timeWindows(0).first;
  int point_index;

  shifts_l->emplace_back(
    Shift(
      shifts_l->size(), // index
      first_driver_l.index(), // driver
      first_driver_l.trailer(), // trailer
      starting_time_l)); // Start

  shifts_l->at(0).operations_ptr()->emplace_back(
    Operation(
      2, // point
      starting_time_l, // arrival
      264, // quantity
      2 // setup_time, random
      ));

  // Test 
  int shift_l,operation_l;
  EXPECT_EQ(rip::tags::get_string(context.solution()->is_admissible(&shift_l,&operation_l)),
    rip::tags::get_string(SHI03_LOADING_AND_DELIVERY_OPERATIONS_TAKE_A_CONSTANT_TIME));
}

/** SHI05_DELIVERY_OPERATIONS_REQUIRE_THE_CUSTOMER_SITE_TO_BE_ACCESSIBLE_FOR_THE_TRAILER
 * One shift, 1 operation, trailer not correct for the site
 */
TEST(Operations_Constraints_Actives, SHI05) {
  // Construction of Context
  Context context("../instances/Instance_V_1.1.xml");

  // Construction of Solution for SHI05
  std::vector<Shift>* shifts_l = context.solution()->shifts();
  Driver const& driver_l = context.data()->drivers()->begin()->second;
  int starting_time_l = driver_l.timeWindows(0).first;
  int setup_time; 
  int point_index;

  shifts_l->emplace_back(
    Shift(
      shifts_l->size(), // index
      driver_l.index(), // driver
      2, // trailer that do not exist
      starting_time_l)); // Start

  point_index = 2;
  setup_time = rip::helpers::setup_time(point_index,*(context.data()));
  shifts_l->at(0).operations_ptr()->emplace_back(
    Operation(
      point_index, // point
      starting_time_l, // arrival
      264, // quantity
      setup_time // setup_time
      ));

  // Test 
  int shift_l,operation_l;
  EXPECT_EQ(rip::tags::get_string(context.solution()->is_admissible(&shift_l,&operation_l)),
    rip::tags::get_string(SHI05_DELIVERY_OPERATIONS_REQUIRE_THE_CUSTOMER_SITE_TO_BE_ACCESSIBLE_FOR_THE_TRAILER));
}

/** SHI11_SOME_PRODUCT_MUST_BE_LOADED_OR_DELIVERED
 * One shift, 1 operation, with the quantity not correct.
 */
TEST(Operations_Constraints_Actives, SHI11) {
  // Construction of Context
  Context context("../instances/Instance_V_1.1.xml");

  // Construction of Solution for SHI11
  std::vector<Shift>* shifts_l = context.solution()->shifts();
  Driver const& driver_l = context.data()->drivers()->begin()->second;
  int starting_time_l = driver_l.timeWindows(0).first;
  int setup_time; 
  int point_index;

  shifts_l->emplace_back(
    Shift(
      shifts_l->size(), // index
      driver_l.index(), // driver
      driver_l.trailer(), // trailer
      starting_time_l)); // Start

  point_index = 2;
  setup_time = rip::helpers::setup_time(point_index,*(context.data()));
  shifts_l->at(0).operations_ptr()->emplace_back(
    Operation(
      point_index, // point
      starting_time_l, // arrival
      - 264, // quantity negative for a customer
      setup_time // setup_time
      ));

  // Test 
  int shift_l,operation_l;
  EXPECT_EQ(rip::tags::get_string(context.solution()->is_admissible(&shift_l,&operation_l)),
    rip::tags::get_string(SHI11_SOME_PRODUCT_MUST_BE_LOADED_OR_DELIVERED));
}

// Step 3. Call RUN_ALL_TESTS() in main().
//
// We do this by linking in src/gtest_main.cc file, which consists of
// a main() function which calls RUN_ALL_TESTS() for us.
//
// This runs all the tests you've defined, prints the result, and
// returns 0 if successful, or 1 otherwise.
//
// Did you notice that we didn't register the tests?  The
// RUN_ALL_TESTS() macro magically knows about all the tests we
// defined.  Isn't this convenient?
