/**
	CxxExcept - Universal Exception Library
	Copyright (C) 2021  Valdemar Lindberg

*/
#ifndef _CXX_EXECPT_CXX_H_
#define _CXX_EXECPT_CXX_H_ 1

#ifndef __cplusplus

#endif

//#include <bfd.h>
#include <cstdlib>
// TODO check if conditional.
#include <cerrno>
#include <cstring>
#include <cxxabi.h>
#include <execinfo.h>
#include <fmt/format.h>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

#define CXXEXCEPT_USE_BACKWARD
#ifdef CXXEXCEPT_USE_BACKWARD
// TODO add options for which backend.
#define BACKWARD_HAS_BFD 1
#define BACKWARD_HAS_UNWIND 1
#include <backward.hpp>
#define CXXEXCEPT_BACKWARD
#endif

#ifdef __cpp_lib_optional
#include <optional>
#define CXXEXCEPT_HAS_OPTIONAL
#endif

#if defined(__linux) || defined(__linux__)
#include <unistd.h>
#ifdef CXXEXCEPT_LINUX
#endif
#endif

#ifdef __ANDROID__
#endif

#ifdef CXXEXCEPT_USE_UNICODE
#include <unicode/unistr.h>
#endif

namespace cxxexcept {
	// TODO add verison value.

	// TODO check if need to handle unicode, locale and etc.
	typedef std::string String;
	typedef std::wstring WString;

#ifdef _CXXEXCEPT_USE_UNICODE
	using ExceptionString = WString;
#else
	using ExceptionString = String;
#endif

	enum class StackColorPalette { StackColorNone, StackColorDefault, StackColorYellow, StackColorPink };

	/**
	 * @brief
	 *
	 * @tparam Text
	 */
	template <class Text> class IExceptionBackTrace {
	  public:
	  public:
		IExceptionBackTrace(void *stack_start_address) : stack_start_address(stack_start_address) {}
		virtual ~IExceptionBackTrace() = default;
		/**
		 * @brief Get the Back Trace object
		 *
		 * @param colorPalette
		 * @return Text
		 */
		virtual Text
		getBackTrace(StackColorPalette colorPalette = StackColorPalette::StackColorNone) const noexcept = 0;

		/**
		 * @brief Get the Stack Tree object
		 *
		 * @param stackDepth
		 * @param colorPalette
		 * @return Text
		 */
		virtual Text
		getStackTree(int stackDepth,
					 StackColorPalette colorPalette = StackColorPalette::StackColorNone) const noexcept = 0;

		void *getStackStartAddress() const noexcept { return this->stack_start_address; }

	  private:
		void *stack_start_address;
	};

	/**
	 * @brief
	 *
	 * @tparam Text
	 */
	template <typename Text = ExceptionString> class ThrowableException : public std::exception {
	  public:
		ThrowableException() {}

		ThrowableException(const ThrowableException &) = default;
		ThrowableException &operator=(const ThrowableException &) = default;
		ThrowableException(ThrowableException &&) = default;
		ThrowableException &operator=(ThrowableException &&) = default;

		const char *getExceptionName() const noexcept { return typeid(*this).name(); }

		virtual Text getName() const {
			char *className = abi::__cxa_demangle(getExceptionName(), nullptr, nullptr, nullptr);
			if (className) {
			}
			Text strName(className);
			free(className);
			return strName;
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
		static String getCommandLine() noexcept {
			String c;
			// std::ifstream("/proc/self/cmdline").read(c, c.size());
			return c;
		}
		static String getEnviornmentVariables() noexcept { return ""; }

		friend ThrowableException &operator>>(ThrowableException &ex, String &text) { return ex; }

	  private:
	};

#if defined(CXXEXCEPT_USE_BACKWARD)
	/**
	 * @brief
	 *
	 * @tparam Text
	 */
	template <class Text> class IExceptioBackwardBackTrace : public IExceptionBackTrace<Text> {
	  public:
		IExceptioBackwardBackTrace() : IExceptionBackTrace<Text>(nullptr) {
			this->resolver = std::make_shared<backward::TraceResolver>();
			this->stackTrace = std::make_shared<backward::StackTrace>();
		}
		virtual ~IExceptioBackwardBackTrace() = default;

		IExceptioBackwardBackTrace(const IExceptioBackwardBackTrace &other) = default;
		IExceptioBackwardBackTrace &operator=(const IExceptioBackwardBackTrace &other) = default;
		IExceptioBackwardBackTrace(IExceptioBackwardBackTrace &&other) = default;
		IExceptioBackwardBackTrace &operator=(IExceptioBackwardBackTrace &&other) = default;

		virtual Text
		getBackTrace(StackColorPalette colorPalette = StackColorPalette::StackColorDefault) const noexcept override {
			return getStackTree(-1, colorPalette);
		}

		virtual Text
		getStackTree(int stackDepth,
					 StackColorPalette colorPalette = StackColorPalette::StackColorDefault) const noexcept override {

			/*	All.	*/
			if (stackDepth < 0) {
				stackDepth = 32;
			}

			/*	Extract stack.	*/
			//	int si = stackTrace->load_here(stackDepth);
			size_t stack_size;
			if (this->getStackStartAddress() != nullptr)
				stack_size = stackTrace->load_from(this->getStackStartAddress(), stackDepth);
			else
				stack_size = stackTrace->load_here(stackDepth);
			resolver->load_stacktrace(*stackTrace);

			// pthread_getname_np
			size_t thread_id = stackTrace->thread_id();

			stackTrace->size();

			/*	Generate the print message.	*/
			std::ostringstream stream;
			backward::Printer p;
			if (colorPalette == StackColorPalette::StackColorDefault)
				p.color_mode = backward::ColorMode::type::always;
			p.print(*stackTrace, stream);

			/*	*/
			String stackTraceStr = std::move(stream.str());
			return stackTraceStr;
		}

	  private:
		std::shared_ptr<backward::TraceResolver> resolver;
		std::shared_ptr<backward::StackTrace> stackTrace;
	};
#endif

#if defined(CXXEXCEPT_USE_BACKWARD)
	template <class T> using ExceptionDefaultBackStackImpl = IExceptioBackwardBackTrace<T>;
#else
	template <class T> class ExceptionDefaultBackStackImpl {};
#endif

	// std::basic_string<Char>
	// std::basic_string<Char>
	// typename Char
	template <class T, class Text = ExceptionString, class BackTrace = ExceptionDefaultBackStackImpl<Text>>
	class StackException : public ThrowableException<Text>, public BackTrace {
		static_assert(std::is_object<Text>::value, "");
		static_assert(std::is_base_of<IExceptionBackTrace<Text>, BackTrace>::value, "BackTrace Class be ");
		// TODO assert the backstrace impl class typr and etc.

	  protected:
		StackException() {
			// TODO save the start address to start stacking from.
		}

	  public:
		StackException(const char *what) : StackException(std::move(Text(what))) {}
		StackException(const Text &what) : StackException() { message = what; }
		StackException(Text &&what) : StackException() { message = what; }
		template <typename... Args>
		StackException(const Text &format, Args &&... args) : StackException(std::move(fmt::format(format, args...))) {}

		StackException(const StackException &) = default;
		StackException &operator=(const StackException &) = default;
		StackException(StackException &&) = default;
		StackException &operator=(StackException &&) = default;

		virtual const char *what() const noexcept override { return message.c_str(); }
		// virtual const Text & what() const  { return message; }

		friend std::istream &operator>>(std::istream &is, StackException &exception) { return is; }
		friend std::ostream &operator<<(std::ostream &os, const StackException &exception) {
			os << exception.what();
			return os;
		}

	  protected:
	  public:
		template <class U> static std::ostream &printStackMessage(const U &ex, std::ostream &o) noexcept {
			o << ex.what() << ex.getStackTree();
			return o;
		}
		template <class U> static void printStackMessage(const U &ex) noexcept {
			printStackMessage(ex, std::cout);
			std::cout << ex.what() << ex.getStackTree();
		}

	  private:
		Text message;
	};

	// TODO rename
	enum class PrintLevelOfInfo {
		Minimal,
		High,
		Advnaced,
		UberAdvanced,
	};

	template <class U> static const char *getExceptionName() noexcept { return typeid(U).name(); }

	/**
	 * @brief Get the Stack Message object
	 *
	 * @tparam U
	 * @param ex
	 * @param levelInfo
	 * @return String
	 */
	template <class U>
	static String getStackMessage(const U &ex, PrintLevelOfInfo levelInfo = PrintLevelOfInfo::Minimal) noexcept {
		static_assert(std::is_base_of<std::exception, U>::value, "Class Must be derived from std::exception");
		const ThrowableException<String> *throwEx = dynamic_cast<const ThrowableException<String> *>(&ex);
		const IExceptionBackTrace<String> *stackEx = dynamic_cast<const IExceptionBackTrace<String> *>(&ex);
		std::ostringstream stream;
		/*	If */
		if (throwEx) {
			stream << throwEx->getName() << ": ";
			stream << throwEx->what();
			stream << throwEx->getEnviornmentVariables();
			stream << throwEx->getCommandLine();
			if (stackEx) {
				stream << std::endl << stackEx->getBackTrace();
			}
		} else {
			/*	A normal std::exception.	*/
			stream << ex.what();
		}

		return stream.str();
	}

	/**
	 * @brief
	 *
	 * @tparam T
	 * @param ex
	 */
	template <class T> static void printStackMessage(const T &ex) noexcept { std::cerr << getStackMessage<T>(ex); }

	using CaptureException = ThrowableException<ExceptionString>;

	class RuntimeException : public StackException<int> {
	  public:
		RuntimeException() : StackException("RuntimeException") {}
		RuntimeException(RuntimeException &&other) = default;
		RuntimeException(const std::string &arg) : StackException(arg) {}
		template <typename... Args>
		RuntimeException(const std::string &format, Args &&... args) : StackException(format, args...) {}
	};

	class PermissionDeniedException : public StackException<int> {
	  public:
		PermissionDeniedException() : StackException("PermissionDeniedException!") {}

		PermissionDeniedException(const std::string &arg) : StackException(arg) {}
		template <typename... Args>
		PermissionDeniedException(const std::string &format, Args &&... args) : StackException(format, args...) {}
	};
	class DivideByZeroException : public StackException<int> {};
	class IOException : public StackException<int> {};
	class PermissionException : public StackException<int> {};
	class InvalidArgumentException : public StackException<int> {
	  public:
		InvalidArgumentException() : StackException("Invalid Argument") {}
		InvalidArgumentException(InvalidArgumentException &&other) = default;
		InvalidArgumentException(const std::string &arg) : StackException(arg) {}
		template <typename... Args>
		InvalidArgumentException(const std::string &format, Args &&... args) : StackException(format, args...) {}
	};
	class NotImplementedException : public StackException<int> {};
	class NotSupportedException : public StackException<int> {
	  public:
		NotSupportedException() : StackException("Not Supported") {}
		NotSupportedException(NotSupportedException &&other) = default;
		NotSupportedException(const std::string &arg) : StackException(arg) {}
		template <typename... Args>
		NotSupportedException(const std::string &format, Args &&... args) : StackException(format, args...) {}
	};
	class IndexOutOfRangeException : public StackException<int> {
	  public:
		IndexOutOfRangeException() : StackException("IndexOutOfRangeException") {}
		IndexOutOfRangeException(IndexOutOfRangeException &&other) = default;
		IndexOutOfRangeException(const std::string &arg) : StackException(arg) {}
		template <typename... Args>
		IndexOutOfRangeException(const std::string &format, Args &&... args) : StackException(format, args...) {}
	};
	class InvalidPointerException : public StackException<int> {
	  public:
		InvalidPointerException() : StackException("IndexOutOfRangeException") {}
		InvalidPointerException(InvalidPointerException &&other) = default;
		InvalidPointerException(const std::string &arg) : StackException(arg) {}
		template <typename... Args>
		InvalidPointerException(const std::string &format, Args &&... args) : StackException(format, args...) {}
	};

	// TODO determine if shall be renamed to ErrnoException
	class SystemException : public StackException<int> {
	  public:
		SystemException() : SystemException(errno) {}
		SystemException(int errno_nr) : StackException(strerror(errno_nr)) {}
		SystemException(SystemException &&other) = default;
		SystemException(const ExceptionString &arg) : StackException(arg) {}
		template <typename... Args>
		SystemException(int errno_nr, const std::string &format, Args &&... args) : StackException(format, args...) {}
	};

} // namespace cxxexcept

#endif