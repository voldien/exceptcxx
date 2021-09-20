#ifndef _EXECPT_CXX_H_
#define _EXECPT_CXX_H_ 1

#include <execinfo.h>
#include <fmt/format.h>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>

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
	typedef std::wstring WString;

	// class Exception;

	class Throwable {};

	// std::basic_string<Char>
	// typename Char
	template <class T, class Text = String> class Exception : public std::exception {
	  protected:
		Exception(void) {
			// TODO save the start address to start stacking from.
			generateStackTrace(32);
		}

	  public:
		Exception(const char *what) : Exception(std::move(Text(what))) {}
		Exception(const Text &what) : Exception() { message = what; }
		Exception(Text &&what) : Exception() { message = what; }
		template <typename... Args>
		Exception(const Text &format, Args &&... args) : Exception(std::move(fmt::format(format, args...))) {}

		Exception(const Exception &) = default;
		Exception &operator=(const Exception &) = default;
		Exception(Exception &&) = default;
		Exception &operator=(Exception &&) = default;

		const Text &getBackTrace(void) const;

		virtual const char *what() const noexcept override { return stackTrace.c_str(); }
		const char *getStackTree(unsigned int stackDepth = 10) const noexcept { return stackTrace.c_str(); }

		/**
		 * @brief
		 *
		 * @return const char*
		 */
		const char *fillStackSource(void) const { return ""; }
		// const Text &fillStackSource(void) const { return this->stackTrace; }

		//	virtual const char *getName(void) const noexcept = 0;
		constexpr const char *getName(void) const noexcept {
			return "";
			// getExceptionName(*this);
		}

		template <class U> static const char *getExceptionName() noexcept { return typeid(U).name(); }
		template <class U> static void printStackMessage(const U &ex) noexcept {
			std::cout << ex.what() << ex.getStackTree();
		}

	  protected:
		// TODO add relative source path or just filename path.
		inline void generateStackTrace(unsigned int stackDepth) {
			// typedef void (Exception<int>::Exception::*Some_fnc_ptr)();
			// Some_fnc_ptr fnc_ptr = &Exception<int>::Exception;

			// TODO add support to be disabled for the release build.
			std::ostringstream ss;
			StackTrace stackTrace;
			TraceResolver resolver;

			stackTrace.load_here(stackDepth);
			resolver.load_stacktrace(stackTrace);

			SnippetFactory snip;
			// auto pddd = snip.get_snippet("exception.cpp", 1, 100);

			// stackTrace.skip_n_firsts(4);

			Printer p;
			p.print(stackTrace);

			// Colorize
			// ColorMode::always;

			unsigned int offsetTrace = 3;
			for (std::size_t i = offsetTrace; i < stackTrace.size(); ++i) {

				ResolvedTrace trace = resolver.resolve(stackTrace[i]);

				auto snipCode = snip.get_snippet(trace.object_filename, trace.source.line, 5);

				std::cout << "#" << i << " " << trace.object_filename << " " << trace.object_function << " ["
						  << trace.addr << "]" << std::endl;

				//			std::cout << snipCode[trace.idx].second << std::endl;

				// auto it = snipCode.begin();
				// for (it; it != snipCode.end(); it++)
				// 	std::cout << (*it).second << std::endl;
			}

			this->stackTrace = std::move(ss.str());
		}

	  public:
		/// proc/self/cmdline
		// GetCommandLineA
		/**
		 * @brief Get the Command Line object
		 *
		 * @return Text
		 */
		// TODO: implement getcommandline
		Text getCommandLine(void) const noexcept {

			std::string c(4096);
			// std::ifstream("/proc/self/cmdline").read(c, c.size());
		}

	  private:
		Text stackTrace;
		Text message;
	};

	class ThrowableException : public Exception<int> {};

	class RuntimeException : public Exception<int> {
	  public:
		RuntimeException(void) : Exception("Not implemented yet!") {}
		RuntimeException(RuntimeException &&other) = default;
		RuntimeException(const std::string &arg) : Exception(arg) {}
		template <typename... Args>
		RuntimeException(const std::string &format, Args &&... args) : Exception(format, args...) {}
		//	virtual const char *getName(void) const noexcept override { return getExceptionName(); }
	};

	class PermissionDeniedException : public Exception<int> {
	  public:
		PermissionDeniedException(void) : Exception("PermissionDeniedException!") {}

		PermissionDeniedException(const std::string &arg) : Exception(arg) {}
		template <typename... Args>
		PermissionDeniedException(const std::string &format, Args &&... args) : Exception(format, args...) {}
		// virtual const char *getName(void) const noexcept override { return getExceptionName(); }
	};

	/**
	 * @brief
	 *
	 */
	class ErrnoException : public Exception<int> {};
	class DivideByZeroException : public Exception<int> {};
	class IOException : public Exception<int> {};
	class PermissionException : public Exception<int> {};
	class InvalidArgumentException : public Exception<int> {};
	class NotImplementedException : public Exception<int> {};
	class NotSupportedException : public Exception<int> {};
	class IndexOutOfRangeException : public Exception<int> {};
	typedef Exception<int> CaptureException;

} // namespace cxxexcept

#endif