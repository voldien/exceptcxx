#include "Exception.hpp"
#include <gtest/gtest.h>

TEST(ThrowException, RunTimeException) {
	ASSERT_THROW(throw cxxexcept::RuntimeException(), cxxexcept::RuntimeException);
}

class ExceptionNameTest : public ::testing::TestWithParam<std::tuple<std::string>> {
  public:
	void throwException(const std::string &name) {
		if (name == "RuntimeException")
			throw cxxexcept::RuntimeException();
	}
};

TEST_P(ExceptionNameTest, Names) {
	auto [expected] = GetParam();

	try {
		throwException(expected);
	} catch (std::exception &ex) {
		ex.what();
		/*	*/
		cxxexcept::ThrowableException &throwable = dynamic_cast<cxxexcept::ThrowableException &>(ex);
		/*	*/
		ASSERT_STREQ(throwable.getExceptionName(), "RuntimeException");
	}
}

INSTANTIATE_TEST_SUITE_P(ThrowException, ExceptionNameTest, ::testing::Values(std::make_tuple("RuntimeException")));

TEST(ThrowException, RunTimeException_Name_Correct) {}

TEST(ThrowException, Assign_operator) {}