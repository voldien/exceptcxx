#include "Exception.hpp"
#include <iostream>

int main(int argc, const char **argv) {
	try {
		throw cxxexcept::RuntimeException("{}", "Error Message");
	} catch (cxxexcept::CaptureException &ex) {
		std::cerr << cxxexcept::getStackMessage(ex);
	}
}

