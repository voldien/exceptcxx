#include<gtest/gtest.h>
#include"Exception.hpp"

TEST(ThrowException, RunTimeException) { ASSERT_THROW(throw cxxexcept::RuntimeException(), cxxexcept::RuntimeException); }