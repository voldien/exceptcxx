FIND_PACKAGE(GTest QUIET)
IF(GTEST_FOUND)
	MESSAGE(STATUS "Gtest found: ${GTEST_BOTH_LIBRARIES}")
	ADD_EXECUTABLE(exceptionUnitTests ExceptionUnitTests.cpp)
	TARGET_LINK_LIBRARIES(exceptionUnitTests PUBLIC cxxexcept ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
	TARGET_INCLUDE_DIRECTORIES(exceptionUnitTests PRIVATE ${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
	SET_TARGET_PROPERTIES(exceptionUnitTests PROPERTIES
		COMPILE_FLAGS " -Wpedantic -Wextra  -Wall -Wno-attributes")
	ADD_DEPENDENCIES(exceptionUnitTests cxxexcept)


	ADD_TEST(NAME UnitTesting
		COMMAND exceptionUnitTests)

	ADD_EXECUTABLE(exceptionTest throwExecption.cpp)
	TARGET_LINK_LIBRARIES(exceptionTest cxxexcept dl)

ELSE()
	MESSAGE(WARNING "Gtest is required in order to compile and run the tests")
ENDIF()

