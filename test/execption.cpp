#include "Exception.hpp"
#include <iostream>

int main(int argc, const char **argv) {
	try {
		cxxexcept::DivideByZeroException a;
		a << "Bad Division";
		throw a;
		throw cxxexcept::RuntimeException("{}", "hello");
	} catch (cxxexcept::CaptureException &ex) {
		std::cout << ex.what();

		std::cout << ex.getName();
	}
}
