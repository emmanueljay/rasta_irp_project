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
/** DRI01_INTER_SHIFTS_DURATION
 * 2 shifts, to close to each other
 */
TEST(Solution_Constraints_Actives, DRI01) {
  // Construction of Context
  Context context("../instances/Instance_V_1.1.xml");

  // Construction of Solution for DRI01
  std::vector<Shift>* shifts_l = context.solution()->shifts();
  Driver const& first_driver_l = context.data()->drivers()->begin()->second;
  int starting_time_l = first_driver_l.timeWindows(0).first;
  int setup_time,time_l; 
  int point_index;

  // First shift
  shifts_l->emplace_back(
    shifts_l->size(), // index
    first_driver_l.index(), // driver
    first_driver_l.trailer(), // trailer
    starting_time_l); // Start

  shifts_l->emplace_back(
    shifts_l->size(), // index
    first_driver_l.index(), // driver
    first_driver_l.trailer(), // trailer
    shifts_l->at(0).end(*(context.data()))+1); // Start

  // Test 
  int shift_l,operation_l;
  EXPECT_EQ(rip::tags::get_string(context.solution()->is_admissible(&shift_l,&operation_l)),
    rip::tags::get_string(DRI01_INTER_SHIFTS_DURATION));
}

/** TL01_DIFFERENT_SHIFTS_OF_THE_SAME_TRAILER_CANNOT_OVERLAP_IN_TIME
 * 2 shifts that overlaps No Way to test this one, as for now driver and truck 
 * are bounded, and so, same that contraint before.
 */
// TEST(Solution_Constraints_Actives, TL01) {
//   // Construction of Context
//   Context context("../instances/Instance_V_1.1.xml");

//   // Construction of Solution for TL01
//   std::vector<Shift>* shifts_l = context.solution()->shifts();
//   Driver const& first_driver_l = context.data()->drivers()->begin()->second;
//   Driver const& second_driver_l = (++(context.data()->drivers()->begin()))->second;
//   int starting_time_l = first_driver_l.timeWindows(0).first;
//   int setup_time,time_l; 
//   int point_index;

//   // First shift
//   shifts_l->emplace_back(
//     shifts_l->size(), // index
//     first_driver_l.index(), // driver
//     first_driver_l.trailer(), // trailer
//     starting_time_l); // Start

//   // Operation in first shift
//   point_index = 2;
//   setup_time = rip::helpers::setup_time(point_index,*(context.data()));
//   shifts_l->at(0).operations_ptr()->emplace_back(
//     point_index, // point
//     starting_time_l, // arrival
//     264, // quantity
//     setup_time // setup_time
//     );

//   shifts_l->emplace_back(
//     shifts_l->size(), // index
//     second_driver_l.index(), // driver
//     first_driver_l.trailer(), // trailer
//     starting_time_l+10); // Start

//   // Test 
//   int shift_l,operation_l;
//   EXPECT_EQ(rip::tags::get_string(context.solution()->is_admissible(&shift_l,&operation_l)),
//     rip::tags::get_string(TL01_DIFFERENT_SHIFTS_OF_THE_SAME_TRAILER_CANNOT_OVERLAP_IN_TIME));
// }


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
