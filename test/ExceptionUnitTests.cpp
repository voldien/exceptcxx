#include "Exception.hpp"
#include <gtest/gtest.h>

TEST(ThrowException, Throwable_Create_Unicode8_Throw_No_Exception) {
	ASSERT_NO_THROW(cxxexcept::ThrowableException<char>());
}

TEST(ThrowException, Throwable_Create_Unicode16_Throw_No_Exception) {
	ASSERT_NO_THROW(cxxexcept::ThrowableException<wchar_t>());
}

TEST(ThrowException, StackException_Unicode8_Create_Throw_No_Exception) {
	ASSERT_NO_THROW(cxxexcept::StackException<char>(""));
}
TEST(ThrowException, StackException_Unicode16_Create_Throw_No_Exception) {
	ASSERT_NO_THROW(cxxexcept::StackException<wchar_t>(L""));
}

TEST(ThrowException, StackException) {
	ASSERT_THROW(throw cxxexcept::StackException<>(""), cxxexcept::StackException<>);
}

TEST(ThrowException, RunTimeException) {
	ASSERT_THROW(throw cxxexcept::RuntimeException(), cxxexcept::RuntimeException);
}

TEST(ThrowException, StackException_StackAddress_Default_Null) {
	cxxexcept::StackException<> ex("");
	ASSERT_EQ(ex.getStackStartAddress(), nullptr);
}

TEST(ThrowException, StackException_CommandLine_None_Empty) {
	cxxexcept::StackException<> ex("");
	ASSERT_FALSE(ex.getCommandLine().empty());
}

class ExceptionNameTest : public ::testing::TestWithParam<std::tuple<std::string>> {
  public:
	void throwException(const std::string &name) {
		if (name == "RuntimeException")
			throw cxxexcept::RuntimeException();
		else if (name == "PermissionDeniedException")
			throw cxxexcept::PermissionDeniedException();
		else if (name == "DivideByZeroException")
			throw cxxexcept::DivideByZeroException();
		else if (name == "IOException")
			throw cxxexcept::IOException();
		else if (name == "PermissionException")
			throw cxxexcept::PermissionException();
		else if (name == "NotImplementedException")
			throw cxxexcept::NotImplementedException();
	}
};

TEST_P(ExceptionNameTest, Names) {
	auto [expected] = GetParam();

	try {
		throwException(expected);
	} catch (std::exception &ex) {
		ex.what();
		/*	*/
		cxxexcept::ThrowableException<cxxexcept::ExceptionChar> &throwable =
			dynamic_cast<cxxexcept::ThrowableException<cxxexcept::ExceptionChar> &>(ex);
		/*	*/
		ASSERT_STREQ(throwable.getName().c_str(), expected.c_str());
	}
}

INSTANTIATE_TEST_SUITE_P(ThrowException, ExceptionNameTest,
						 ::testing::Values(std::make_tuple("cxxexcept::RuntimeException")));

TEST(ThrowException, RunTimeException_Name_Correct) {}

TEST(ThrowException, Assign_operator) {}