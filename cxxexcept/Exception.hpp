#ifndef _EXECPT_CXX_H_
#define _EXECPT_CXX_H_ 1
#include <backward.hpp>
#include <execinfo.h>
#include <fmt/format.h>
#include <sstream>
#include <stdexcept>

namespace cxxexcept {
	using namespace backward;
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
		const char *getStackTree(void) const { return ""; }

		virtual const char *getName(void) const noexcept = 0;

	  protected:
		void generateStackTrace(void) {
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

	  private:
		Text stackTrace;
		Text message;
	};

	typedef Exception<int> CaptureException;

	class RuntimeException : public Exception<int> {
	  public:
		RuntimeException(void) : Exception("Not implemented yet!") {}
		RuntimeException(RuntimeException &&other) = default;
		RuntimeException(const std::string &arg) : Exception(arg) {}
		template <typename... Args>
		RuntimeException(const std::string &format, Args &&... args) : Exception(format, args...) {}
		virtual const char *getName(void) const noexcept override { return typeid(this).name(); }
	};

	class PermissionDeniedException : public Exception<int> {
	  public:
		PermissionDeniedException(void) : Exception("PermissionDeniedException!") {}

		PermissionDeniedException(const std::string &arg) : Exception(arg) {}
		template <typename... Args>
		PermissionDeniedException(const std::string &format, Args &&... args) : Exception(format, args...) {}
		virtual const char *getName(void) const noexcept override { return typeid(this).name(); }
	};

} // namespace cxxexcept

#endif