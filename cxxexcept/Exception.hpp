/**
	CxxExcept - Universal Exception Library
	Copyright (C) 2018  Valdemar Lindberg

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef _CXX_EXECPT_CXX_H_
#define _CXX_EXECPT_CXX_H_ 1

#include <bfd.h>
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

#define BACKWARD_HAS_BFD 1
#define BACKWARD_HAS_UNWIND 1
//#define BACKWARD_HAS_BACKTRACE_SYMBOL 1
#include <backward.hpp>
#ifdef CXXEXCEPT_USE_BACKWARD

#define CXXEXCEPT_BACKWARD
#endif

#ifdef __cpp_lib_optional
#include <optional>
#define CXXEXCEPT_HAS_OPTIONAL
#endif

#if defined(__linux) || defined(__linux__)
#include <unistd.h>
#endif

#ifdef __ANDROID__
#endif

//#include <unicode/unistr.h>

namespace cxxexcept {
	using namespace backward;

	// TODO add verison value.

	// TODO check if need to handle unicode, locale and etc.
	typedef std::string String;
	typedef std::wstring WString;

#ifdef _CXXEXCEPT_USE_UNICODE
	typedef WString ExceptionString;
#else
	typedef String ExceptionString;

#endif

	class ThrowableException : public std::exception {
	  public:
		ThrowableException() {}

		ThrowableException(const ThrowableException &) = default;
		ThrowableException &operator=(const ThrowableException &) = default;
		ThrowableException(ThrowableException &&) = default;
		ThrowableException &operator=(ThrowableException &&) = default;

		enum StackColorPalette {

		};

		const char *getExceptionName() const noexcept { return typeid(*this).name(); }

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

	// class Exception;
	template <typename Text = ExceptionString> class ExceptionBase : public ThrowableException {

		virtual Text getName() const {
			const char *className = abi::__cxa_demangle(getExceptionName(), nullptr, nullptr, nullptr);
			Text strName(className);
			free((void *)className);
			return strName;
		}

		// virtual const Text &getBackTrace() const = 0;
		// virtual Text getStackTree(unsigned int stackDepth) const noexcept = 0;

	  private:
		Text text;
	};

	template <class Text> class IExceptionBackTrace {
	  public:
		IExceptionBackTrace() {}

		IExceptionBackTrace(const IExceptionBackTrace &) = default;
		IExceptionBackTrace &operator=(const IExceptionBackTrace &) = default;
		IExceptionBackTrace(IExceptionBackTrace &&) = default;
		IExceptionBackTrace &operator=(IExceptionBackTrace &&) = default;

		virtual const Text &getBackTrace() const { return getStackTree(-1); }

		virtual Text getStackTree(int stackDepth) const noexcept {
			/*	Extract stack.	*/
			TraceResolver resolver;
			StackTrace stackTrace;
			int si = stackTrace.load_here(stackDepth);
			resolver.load_stacktrace(stackTrace);

			/*	Generate the print message.	*/
			std::ostringstream stream;
			Printer p;
			p.print(stackTrace, stream);
			String stackTraceStr = std::move(stream.str());
			return stackTraceStr;
		}

	  private:
	};

	// std::basic_string<Char>
	// std::basic_string<Char>
	// typename Char
	template <class T, class Text = ExceptionString, class BackTrace = IExceptionBackTrace<Text>>
	class StackException : public ExceptionBase<ExceptionString>, public BackTrace {
		static_assert(std::is_object<Text>::value, "");
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

		// virtual const Text &getBackTrace() const override {}

		// virtual Text getStackTree(unsigned int stackDepth) const noexcept { return Text(stackTrace.c_str()); }

		friend std::istream &operator>>(std::istream &is, StackException &t) {}

		friend std::ostream &operator<<(std::ostream &os, const StackException &t) {}

		/**
		 * @brief
		 *
		 * @return const char*
		 */
		const char *fillStackSource() const { return ""; }
		// const Text &fillStackSource() const { return this->stackTrace; }

	  protected:
	  public:
		template <class U> static std::ostream &printStackMessage(const U &ex, std::ostream &o) noexcept {
			o << ex.what() << ex.getStackTree();
		}
		template <class U> static void printStackMessage(const U &ex) noexcept {
			printStackMessage(ex, std::cout);
			std::cout << ex.what() << ex.getStackTree();
		}

	  private:
		Text stackTrace;
		Text message;
	};

	// class ThrowableException : public Exception<int> {};

	class RuntimeException : public StackException<int> {
	  public:
		RuntimeException() : StackException("Not implemented yet!") {}
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

	/**
	 * @brief
	 *
	 */
	class ErrnoException : public StackException<int> {};
	class DivideByZeroException : public StackException<int> {};
	class IOException : public StackException<int> {};
	class PermissionException : public StackException<int> {};
	class InvalidArgumentException : public StackException<int> {
	  public:
		InvalidArgumentException() : StackException("Invalid Argument!") {}
		InvalidArgumentException(InvalidArgumentException &&other) = default;
		InvalidArgumentException(const std::string &arg) : StackException(arg) {}
		template <typename... Args>
		InvalidArgumentException(const std::string &format, Args &&... args) : StackException(format, args...) {}
	};
	class NotImplementedException : public StackException<int> {};
	class NotSupportedException : public StackException<int> {
	  public:
		NotSupportedException() : StackException("Invalid Argument!") {}
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
		SystemException(int err) : StackException(strerror(errno)) {}
		SystemException(SystemException &&other) = default;
		SystemException(const ExceptionString &arg) : StackException(arg) {}
		template <typename... Args>
		SystemException(const ExceptionString &format, Args &&... args) : StackException(format, args...) {}
	};

	// TODO rename
	enum class PrintLevelOfInfo {
		Minimal,
		High,
		Advnaced,
		UberAdvanced,
	};

	template <class U> static const char *getExceptionName() noexcept { return typeid(U).name(); }

	template <class U>
	static String getStackMessage(const U &ex, PrintLevelOfInfo levelInfo = PrintLevelOfInfo::Minimal) noexcept {
		static_assert(std::is_base_of<std::exception, U>::value, "Class Must be derived from std::exception");
		const ThrowableException *throwEx = dynamic_cast<const ThrowableException *>(&ex);
		const IExceptionBackTrace<String> *stackEx = dynamic_cast<const IExceptionBackTrace<String> *>(&ex);
		std::ostringstream stream;
		/*	If */
		if (throwEx) {
			stream << throwEx->getExceptionName();
			stream << throwEx->what();
			stream << throwEx->getEnviornmentVariables();
			stream << throwEx->getCommandLine();
			stream << stackEx->getBackTrace();
			stream << stackEx->getBackTrace();
		} else {
			/*	A normal std::exception.	*/
			stream << ex.what();
		}

		return stream.str();
	}

	template <class T> static void printStackMessage(const T &ex) noexcept { std::cerr << getStackMessage<T>(ex); }

	// using enable_if_t = typename std::enable_if<B, T>::type;
	using CaptureException = ThrowableException;

} // namespace cxxexcept

#endif