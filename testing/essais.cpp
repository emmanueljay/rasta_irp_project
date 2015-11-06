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
//
// </TechnicalDetails>


// Tests Factorial().

// Tests factorial of negative numbers.
// TEST(FactorialTest, Negative) {
//   // This test is named "Negative", and belongs to the "FactorialTest"
//   // test case.
//   EXPECT_EQ(1, Factorial(-5));
//   EXPECT_EQ(1, Factorial(-1));
//   EXPECT_GT(Factorial(-10), 0);

//   // <TechnicalDetails>
//   //
//   // EXPECT_EQ(expected, actual) is the same as
//   //
//   //   EXPECT_TRUE((expected) == (actual))
//   //
//   // except that it will print both the expected value and the actual
//   // value when the assertion fails.  This is very helpful for
//   // debugging.  Therefore in this case EXPECT_EQ is preferred.
//   //
//   // On the other hand, EXPECT_TRUE accepts any Boolean expression,
//   // and is thus more general.
//   //
//   // </TechnicalDetails>
// }

// // Tests factorial of 0.
// TEST(FactorialTest, Zero) {
//   EXPECT_EQ(1, Factorial(0));
// }

// // Tests factorial of positive numbers.
// TEST(FactorialTest, Positive) {
//   EXPECT_EQ(1, Factorial(1));
//   EXPECT_EQ(2, Factorial(2));
//   EXPECT_EQ(6, Factorial(3));
//   EXPECT_EQ(40320, Factorial(8));
// }


// SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT
TEST(ConstraintActives, SHI02) {
  // Construction of Context
  google::InitGoogleLogging("test_essais");
  Context context("../instances/Instance_V_1.1.xml");

  // Construction of Solution for SHI02
  std::vector<Shift>* shifts_l = context.solution()->shifts();
  Driver const& first_driver_l = context.data()->drivers()->begin()->second;

  shifts_l->emplace_back(
    Shift(
      shifts_l->size(), // index
      first_driver_l.index(), // driver
      first_driver_l.trailer(), // trailer
      0));

  // Test 
  int shift_l,operation_l;
  EXPECT_EQ(rip::tags::get_string(context.solution()->is_admissible(&shift_l,&operation_l)),
    rip::tags::get_string(SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT));
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
