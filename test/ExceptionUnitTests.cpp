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
	ASSERT_NE(ex.getStackStartAddress(), nullptr);
}

TEST(ThrowException, StackException_CommandLine_None_Empty) {
	cxxexcept::StackException<> ex("");
	ASSERT_FALSE(ex.getCommandLine().empty());
}

TEST(ThrowException, StackException_CommandLine_Correct) {
	cxxexcept::StackException<> ex("");
	// ASSERT_EQ(ex.getCommandLine(), "exceptionUnitTests");
}

TEST(ThrowException, ErrnoException) {
	//	ASSERT_MP_THROW (cxxexcept::SystemException(0, std::system_category(), "{}". "Error"));
}

template <class T> class ExceptionTypeTest : public ::testing::Test {};
TYPED_TEST_SUITE_P(ExceptionTypeTest);

TYPED_TEST_P(ExceptionTypeTest, DefaultConstructor) { ASSERT_NO_THROW(TypeParam()); }

TYPED_TEST_P(ExceptionTypeTest, Throw_Correct_Exception) { ASSERT_THROW(throw TypeParam(), TypeParam); }

TYPED_TEST_P(ExceptionTypeTest, Move_Semantics_Correct) {
	TypeParam excep;
	ASSERT_NO_THROW(TypeParam moved = std::move(excep));
}

REGISTER_TYPED_TEST_SUITE_P(ExceptionTypeTest, DefaultConstructor, Throw_Correct_Exception, Move_Semantics_Correct);

using DefaultExceptionTypes = ::testing::Types<cxxexcept::RuntimeException, cxxexcept::PermissionDeniedException,
											   cxxexcept::DivideByZeroException, cxxexcept::IOException,
											   cxxexcept::InvalidArgumentException, cxxexcept::NotImplementedException>;
INSTANTIATE_TYPED_TEST_SUITE_P(DefualtExceptionTypes, ExceptionTypeTest, DefaultExceptionTypes);

using ThrowableExceptionUnicodeTypes =
	::testing::Types<cxxexcept::StackException<char>, cxxexcept::StackException<wchar_t>>;
INSTANTIATE_TYPED_TEST_SUITE_P(ExceptionTypeTestSS, ExceptionTypeTest, ThrowableExceptionUnicodeTypes);

class ExceptionNameTest : public ::testing::TestWithParam<std::tuple<std::string>> {
  public:
	void throwException(const std::string &name) {
		if (name == "RuntimeException") {
			throw cxxexcept::RuntimeException();
		} if (name == "PermissionDeniedException") {
			throw cxxexcept::PermissionDeniedException();
		} if (name == "DivideByZeroException") {
			throw cxxexcept::DivideByZeroException();
		} if (name == "IOException") {
			throw cxxexcept::IOException();
		} if (name == "NotImplementedException") {
			throw cxxexcept::NotImplementedException();
}
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

