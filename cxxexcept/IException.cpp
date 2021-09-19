#include "IException.h"
#include <backward.hpp>
#include <execinfo.h>
#include <sstream>
using namespace execptcxx;
using namespace backward;

IException::IException(void) { this->generateStackTrace(); }

IException::IException(const char *what) {
	this->generateStackTrace();
	this->mwhat = std::string(what);
}

const std::string &IException::getBackTrace(void) const { return this->stackTrace; }

void IException::generateStackTrace(void) {


	// TODO add support to be disabled for the release build.
	std::ostringstream ss;
	StackTrace stackTrace;
	TraceResolver resolver;
	stackTrace.load_here();
	resolver.load_stacktrace(stackTrace);

	for (std::size_t i = 0; i < stackTrace.size(); ++i) {
		const ResolvedTrace trace = resolver.resolve(stackTrace[i]);
		ss << "#" << i << " at " << trace.object_function << "\n";
	}

	this->stackTrace = std::move(ss.str());
}

const char *IException::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT { return this->mwhat.c_str(); }
