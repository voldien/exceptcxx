/*
 * Copyright (c) 2021 Valdemar Lindberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef _CXX_EXECPT_CXX_H_
#define _CXX_EXECPT_CXX_H_ 1

#ifndef __cplusplus

#endif

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
	using ExceptionChar = wchar_t;
#else
	using DefaultExcepCXXString = String;
	using ExceptionChar = char;
#endif

	enum class StackColorPalette { StackColorNone, StackColorDefault, StackColorYellow, StackColorPink };

	/**
	 * @brief
	 *
	 * @tparam Text
	 */
	template <typename Char, class Text = std::basic_string<Char>> class IExceptionBackTrace {
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
	template <typename Char, class Text = std::basic_string<Char>> class ThrowableException : public std::exception {
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
			std::string Strname(className);
			free(className);
			Text exceptionName(Strname.begin(), Strname.end());
			return exceptionName;
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
		static Text getCommandLine() noexcept {
			String c;
			std::ifstream cmd("/proc/self/cmdline");
			std::getline(cmd, c);
			cmd.close();
			return Text(c.begin(), c.end());
		}
		static Text getEnviornmentVariables() noexcept { return static_cast<Text>(""); }

		friend ThrowableException &operator>>(ThrowableException &ex, String &text) { return ex; }

	  private:
	};

#if defined(CXXEXCEPT_USE_BACKWARD)
	/**
	 * @brief
	 *
	 * @tparam Text
	 */
	template <typename Char, class Text = std::basic_string<Char>>
	class IExceptioBackwardBackTrace : public IExceptionBackTrace<Char> {
	  public:
		IExceptioBackwardBackTrace() : IExceptionBackTrace<Char>(nullptr) {
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
			std::string stackTraceStr = stream.str();
			Text stackTracestring(stackTraceStr.begin(), stackTraceStr.end());
			return stackTracestring;
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

	template <typename Char = ExceptionChar, class Text = std::basic_string<Char>,
			  class BackTrace = ExceptionDefaultBackStackImpl<Char>>
	class StackException : public ThrowableException<Char>, public BackTrace {
		static_assert(std::is_object<Text>::value, "");
		static_assert(std::is_base_of<IExceptionBackTrace<Char>, BackTrace>::value, "BackTrace Class be ");
		// TODO assert the backstrace impl class typr and etc.

	  protected:
		StackException() {
			// TODO save the start address to start stacking from.
		}

	  public:
		StackException(const Char *what) : StackException(std::move(Text(what))) {}
		StackException(const Text &what) : StackException() { message = what; }
		StackException(Text &&what) : StackException() { message = what; }
		template <typename... Args>
		StackException(const Text &format, Args &&... args) : StackException(std::move(fmt::format(format, args...))) {}

		StackException(const StackException &) = default;
		StackException &operator=(const StackException &) = default;
		StackException(StackException &&) = default;
		StackException &operator=(StackException &&) = default;

		virtual const char *what() const noexcept override { return (const char *)message.c_str(); }
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
		const ThrowableException<char> *throwEx = dynamic_cast<const ThrowableException<ExceptionChar> *>(&ex);
		const IExceptionBackTrace<char> *stackEx = dynamic_cast<const IExceptionBackTrace<char> *>(&ex);
		std::basic_ostringstream<char> stream;

		/*	If */
		if (throwEx) {
			stream << throwEx->getName() << ": " << throwEx->what() << std::endl;
			stream << "Environment Variables: " << throwEx->getEnviornmentVariables() << std::endl;
			stream << "Command: " << throwEx->getCommandLine() << std::endl;

			/*	If it has a exception backtrace.	*/
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

	using CaptureException = ThrowableException<ExceptionChar>;

	class RuntimeException : public StackException<> {
	  public:
		RuntimeException() : StackException("RuntimeException") {}
		RuntimeException(RuntimeException &&other) = default;
		RuntimeException(const DefaultExcepCXXString &arg) : StackException(arg) {}
		template <typename... Args>
		RuntimeException(const DefaultExcepCXXString &format, Args &&... args) : StackException(format, args...) {}
	};

	class PermissionDeniedException : public StackException<> {
	  public:
		PermissionDeniedException() : StackException("PermissionDeniedException!") {}

		PermissionDeniedException(const DefaultExcepCXXString &arg) : StackException(arg) {}
		template <typename... Args>
		PermissionDeniedException(const DefaultExcepCXXString &format, Args &&... args)
			: StackException(format, args...) {}
	};
	class DivideByZeroException : public StackException<> {};
	class IOException : public StackException<> {};
	class PermissionException : public StackException<> {};
	class InvalidArgumentException : public StackException<> {
	  public:
		InvalidArgumentException() : StackException("Invalid Argument") {}
		InvalidArgumentException(InvalidArgumentException &&other) = default;
		InvalidArgumentException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		InvalidArgumentException(const DefaultExcepCXXString &format, Args &&... args)
			: StackException(format, args...) {}
	};
	class NotImplementedException : public StackException<> {};
	class NotSupportedException : public StackException<> {
	  public:
		NotSupportedException() : StackException("Not Supported") {}
		NotSupportedException(NotSupportedException &&other) = default;
		NotSupportedException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		NotSupportedException(const DefaultExcepCXXString &format, Args &&... args) : StackException(format, args...) {}
	};
	class IndexOutOfRangeException : public StackException<> {
	  public:
		IndexOutOfRangeException() : StackException("IndexOutOfRangeException") {}
		IndexOutOfRangeException(IndexOutOfRangeException &&other) = default;
		IndexOutOfRangeException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		IndexOutOfRangeException(const DefaultExcepCXXString &format, Args &&... args)
			: StackException(format, args...) {}
	};
	class InvalidPointerException : public StackException<> {
	  public:
		InvalidPointerException() : StackException("IndexOutOfRangeException") {}
		InvalidPointerException(InvalidPointerException &&other) = default;
		InvalidPointerException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		InvalidPointerException(const DefaultExcepCXXString &format, Args &&... args)
			: StackException(format, args...) {}
	};

	// TODO determine if shall be renamed to ErrnoException
	class SystemException : public StackException<> {
	  public:
		SystemException() : SystemException(errno) {}
		SystemException(int errno_nr) : StackException(strerror(errno_nr)) {}
		SystemException(SystemException &&other) = default;
		SystemException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		SystemException(int errno_nr, const DefaultExcepCXXString &format, Args &&... args)
			: StackException(format, args...) {}
	};

} // namespace cxxexcept

#endif