#include "Exception.hpp"
#include <iostream>

int main(int argc, const char **argv) {
	try {
		throw cxxexcept::RuntimeException("{}", "hello");
	} catch (cxxexcept::CaptureException &ex) {
		std::cout << ex.what();

		std::cout << ex.getName();
	}
}
