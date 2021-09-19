#ifndef _EXECPT_CXX_H_
#define _EXECPT_CXX_H_ 1

#include <execinfo.h>
#include <fmt/format.h>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

#include <bfd.h>
#define BACKWARD_HAS_BFD 1
#define BACKWARD_HAS_UNWIND 1
#define BACKWARD_HAS_BACKTRACE_SYMBOL 1
#include <backward.hpp>
#ifdef CXXEXCEPT_USE_BACKWARD

#define CXXEXCEPT_BACKWARD
#endif

#ifdef __cpp_lib_optional
#include <optional>
#define CXXEXCEPT_HAS_OPTIONAL
#endif

//#include <unicode/unistr.h>

namespace cxxexcept {
	using namespace backward;

	// TODO add verison value.

	typedef std::string String;

	template <class T> const char *getExceptionName(const T &exception) { return typeid(T).name(); }

	// std::basic_string<Char>
	// typename Char
	template <class T, class Text = std::string> class Exception : public std::exception {
	  protected:
		Exception(void) { generateStackTrace(); }

	  public:
		Exception(const char *what) : Exception(std::string(what)) {}
		Exception(const Text &what) : Exception() { message = what; }
		template <typename... Args>
		Exception(const Text &format, Args &&... args) : Exception(fmt::format(format, args...)) {}

		Exception(const Exception &) = default;
		Exception &operator=(const Exception &) = default;
		Exception(Exception &&) = default;
		Exception &operator=(Exception &&) = default;

		const Text &getBackTrace(void) const;

		const char *what(void) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT { return stackTrace.c_str(); }
		const char *getStackTree(unsigned int stackDepth = 10) const noexcept { return stackTrace.c_str(); }
		const char *fillStackSource(void) const { return ""; }

		//	virtual const char *getName(void) const noexcept = 0;
		virtual const char *getName(void) const noexcept { return getExceptionName(this); }

	  protected:
		inline void generateStackTrace(void) {
			// TODO add support to be disabled for the release build.
			std::ostringstream ss;
			StackTrace stackTrace;
			TraceResolver resolver;

			stackTrace.load_here();
			resolver.load_stacktrace(stackTrace);

			SnippetFactory snip;
			// auto pddd = snip.get_snippet("exception.cpp", 1, 100);

			unsigned int offsetTrace = 3;
			for (std::size_t i = offsetTrace; i < stackTrace.size(); ++i) {

				ResolvedTrace trace = resolver.resolve(stackTrace[i]);

				auto snipCode = snip.get_snippet(trace.object_filename, trace.source.line, 5);

				std::cout << "#" << i << " " << trace.object_filename << " " << trace.object_function << " ["
						  << trace.addr << "]" << snipCode[0].second << std::endl;
			}

			this->stackTrace = std::move(ss.str());
		}

	  private:
		Text stackTrace;
		Text message;
	};

	class RuntimeException : public Exception<int> {
	  public:
		RuntimeException(void) : Exception("Not implemented yet!") {}
		RuntimeException(RuntimeException &&other) = default;
		RuntimeException(const std::string &arg) : Exception(arg) {}
		template <typename... Args>
		RuntimeException(const std::string &format, Args &&... args) : Exception(format, args...) {}
		virtual const char *getName(void) const noexcept override { return getExceptionName(this); }
	};

	class PermissionDeniedException : public Exception<int> {
	  public:
		PermissionDeniedException(void) : Exception("PermissionDeniedException!") {}

		PermissionDeniedException(const std::string &arg) : Exception(arg) {}
		template <typename... Args>
		PermissionDeniedException(const std::string &format, Args &&... args) : Exception(format, args...) {}
		virtual const char *getName(void) const noexcept override { return getExceptionName(this); }
	};

	/**
	 * @brief
	 *
	 */
	class ErrnoException : public Exception<int> {};
	class DivideByZeroException : public Exception<int> {};
	typedef Exception<int> CaptureException;

	template<class T>
	void printStackMessage(Exception<T>& ex){

	}

} // namespace cxxexcept

#endif