# Asserting the functonality.
#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/assert)


FIND_PACKAGE(GTest REQUIRED)
IF(GTEST_FOUND)
	MESSAGE(STATUS "Gtest found: ${GTEST_BOTH_LIBRARIES}")
ELSE()
	MESSAGE(ERROR "Gtest is required in order to compile and run the tests")
ENDIF()

ADD_EXECUTABLE(exceptionUnitTests ExceptionUnitTests.cpp)
TARGET_LINK_LIBRARIES(exceptionUnitTests PUBLIC cxxexcept ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread bfd)
TARGET_INCLUDE_DIRECTORIES(exceptionUnitTests PRIVATE ${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
ADD_DEPENDENCIES(exceptionUnitTests cxxexcept)


ADD_TEST(NAME UnitTesting
    COMMAND exceptionUnitTests)


ADD_EXECUTABLE(exceptionTest throwExecption.cpp)
TARGET_LINK_LIBRARIES(exceptionTest cxxexcept dl bfd)
