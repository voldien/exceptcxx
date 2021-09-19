# Asserting the functonality.
#ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/assert)
ADD_EXECUTABLE(exceptionTest execption.cpp)
TARGET_LINK_LIBRARIES(exceptionTest cxxexcept dl bfd)
