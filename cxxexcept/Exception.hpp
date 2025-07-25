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
#include <iostream>
// TODO check if conditional.
#include <cerrno>
#include <cstring>
#include <cxxabi.h>
#include <execinfo.h>
#include <fmt/format.h>
#include <memory>
#include <sstream>
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
#if defined(__unix__) || defined(__unix) || defined(unix) /*  Unix    */
#include <unistd.h>
#define CXXEXCEPT_UNIX
#endif

#if defined(__linux) || defined(__linux__)
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
	using String = std::string;
	using WString = std::wstring;

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

		static  void *getCurrentAddress() noexcept {
			const static int BT_BUF_SIZE = 100;
			void *buffer[BT_BUF_SIZE];
			int nptrs = backtrace(buffer, BT_BUF_SIZE);
			return buffer[nptrs - 2];
		}
		static  int getStackSize() noexcept {
			const static int BT_BUF_SIZE = 100;
			void *buffer[BT_BUF_SIZE];
			int nptrs = backtrace(buffer, BT_BUF_SIZE);
			return nptrs;
		}

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
		using ExceptionChar = Char;
		using ExceptionText = Text;

		ThrowableException() {}

		ThrowableException(const ThrowableException &) = default;
		ThrowableException &operator=(const ThrowableException &) = default;
		ThrowableException(ThrowableException &&) = default;
		ThrowableException &operator=(ThrowableException &&) = default;

		const char *getExceptionName() const noexcept { return typeid(*this).name(); }

		virtual Text getName() const {
			const char *exceptionName = this->getExceptionName();
			char *className = abi::__cxa_demangle(exceptionName, nullptr, nullptr, nullptr);
			Text _finalExceptionName;
			if (className == nullptr) {
				std::string Strname(exceptionName);
				_finalExceptionName = Text(Strname.begin(), Strname.end());
			} else {
				std::string Strname(className);
				free(className);
				_finalExceptionName = Text(Strname.begin(), Strname.end());
			};
			return _finalExceptionName;
		}

		// GetCommandLineA
		/**
		 * @brief Get the Command Line object
		 *
		 * @return Text
		 */
		// TODO: implement getcommandline
		static Text getCommandLine() noexcept {
#ifdef CXXEXCEPT_UNIX

			String cmd_output;
			std::ifstream cmd("/proc/self/cmdline");
			std::getline(cmd, cmd_output);
			cmd.close();
			return Text(cmd_output.begin(), cmd_output.end());
#else
			return Text();
#endif
		}
		static Text getEnviornmentVariables() noexcept {

			std::ostringstream stream;
			for (char **env = environ; *env != nullptr; env++) {
				char *thisEnv = *env;
				stream << thisEnv;
			}
			std::string stackTraceStr = stream.str();
			Text stackTracestring(stackTraceStr.begin(), stackTraceStr.end());
			return stackTracestring;
		}

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
		IExceptioBackwardBackTrace() : IExceptionBackTrace<Char>(IExceptionBackTrace<Char>::getCurrentAddress()) {
			this->resolver = std::make_shared<backward::TraceResolver>();
			this->stackTrace = std::make_shared<backward::StackTrace>();
			/*	Load Stack.	*/
			this->loadStack(this->getStackStartAddress(), -1);
		}
		~IExceptioBackwardBackTrace() override = default;

		IExceptioBackwardBackTrace(const IExceptioBackwardBackTrace &other) = default;
		IExceptioBackwardBackTrace &operator=(const IExceptioBackwardBackTrace &other) = default;
		IExceptioBackwardBackTrace(IExceptioBackwardBackTrace &&other) = default;
		IExceptioBackwardBackTrace &operator=(IExceptioBackwardBackTrace &&other) = default;

		Text
		getBackTrace(StackColorPalette colorPalette = StackColorPalette::StackColorDefault) const noexcept override {
			return getStackTree(-1, colorPalette);
		}

		Text
		getStackTree(int stackDepth,
					 StackColorPalette colorPalette = StackColorPalette::StackColorDefault) const noexcept override {

			resolver->load_stacktrace(*stackTrace);

			// TODO resolve what do to with it
			(void)stackDepth;

			/*	Generate the print message.	*/
			std::ostringstream stream;
			backward::Printer printer;
			if (colorPalette == StackColorPalette::StackColorDefault) {
				printer.color_mode = backward::ColorMode::type::automatic;
			} else if (colorPalette == StackColorPalette::StackColorNone) {
				printer.color_mode = backward::ColorMode::type::never;
			} else {
				printer.color_mode = backward::ColorMode::type::always;
			}
			printer.print(*stackTrace, stream);

			/*	Generate StackTrace String.	*/
			std::string stackTraceStr = stream.str();
			Text stackTracestring(stackTraceStr.begin(), stackTraceStr.end());
			return stackTracestring;
		}

	  protected:
		size_t loadStack(void *address, int32_t stackDepth) {

			if (stackDepth < 0) {
				stackDepth = 32;
			}
			size_t size;
			if (address != nullptr) {
				size = stackTrace->load_from(address, stackDepth);
				if (size == 0) {
					size = stackTrace->load_here(stackDepth);
				}
			} else {
				size = stackTrace->load_here(stackDepth);
			}
			/*	TODO compute the offset to get the function that invoked throw*/
			int offset_address = 8;
			stackTrace->skip_n_firsts(offset_address);

			return size;
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
		static_assert(std::is_object<Text>::value, "Invalid");
		static_assert(std::is_base_of<IExceptionBackTrace<Char>, BackTrace>::value, "BackTrace Class be ");
		static_assert(std::is_base_of<Text, std::basic_string<Char>>::value, "Invalid");
		// TODO assert the backstrace impl class type and etc.

	  public:
		StackException() {
			// TODO save the start address to start stacking from.
		}

		StackException(const Char *what) : StackException(std::move(Text(what))) {}
		StackException(const Text &what) : StackException() { this->setMessage(what); }
		StackException(Text &&what) : StackException() { this->setMessage(what); }
		template <typename... Args>
		StackException(const Text &format, Args &&...args) : StackException(std::move(fmt::format(format, args...))) {}

		StackException(const StackException &) = default;
		StackException &operator=(const StackException &) = default;
		StackException(StackException &&) = default;
		StackException &operator=(StackException &&) = default;

		const char *what() const noexcept override { return message.c_str(); }

		friend std::ostream &operator<<(std::ostream &outStream, const StackException &exception) {
			outStream << exception.what();
			return outStream;
		}

	  protected:
	  public:
		template <class U>
		static std::ostream &printStackMessage(const U &exception, std::ostream &outStream) noexcept {
			outStream << exception.what() << exception.getStackTree();
			return outStream;
		}
		template <class U> static void printStackMessage(const U &exception) noexcept {
			printStackMessage(exception, std::cout);
			std::cout << exception.what() << exception.getStackTree();
		}

		void setMessage(const Text &text) {
#ifdef __cpp_constexpr
			if constexpr (std::is_same<Text, String>::value) {
				this->message = text;
			} else {
				this->message = String(text.begin(), text.end());
			}
#else
			if (std::is_same<Text, String>::value) {
				Text *pMessg = reinterpret_cast<Text *>(&message);
				*pMessg = text;
			} else {
				this->message = String(text.begin(), text.end());
			}
#endif
		}

	  private:
		String message;
	};

	// TODO rename
	enum class PrintLevelOfInfo { Minimal = 0, High = 1, Advanced = 2, UberAdvanced = 3, Environment = (1 << 8) };

	template <class U> static const char *getExceptionName() noexcept { return typeid(U).name(); }

	/**
	 * @brief Get the Stack Message object
	 *
	 * @tparam U
	 * @param ex
	 * @param levelInfo
	 */
	template <class U>
	static String getStackMessage(const U &exception, PrintLevelOfInfo levelInfo = PrintLevelOfInfo::Minimal) noexcept {
		static_assert(std::is_base_of<std::exception, U>::value, "Class Must be derived from std::exception");
		const ThrowableException<char> *throwEx = dynamic_cast<const ThrowableException<ExceptionChar> *>(&exception);
		const IExceptionBackTrace<char> *stackEx = dynamic_cast<const IExceptionBackTrace<char> *>(&exception);
		std::basic_ostringstream<char> stream;

		/*	If */
		if (throwEx) {
			if (levelInfo == PrintLevelOfInfo::Minimal) {
				stream << throwEx->getName() << ": " << throwEx->what() << std::endl;
			}
			bool showEnvVar =
				static_cast<unsigned int>(levelInfo) & static_cast<unsigned int>(PrintLevelOfInfo::Environment);
			if (showEnvVar) {
				stream << "Environment Variables: " << ThrowableException<char>::getEnviornmentVariables() << std::endl;
			}
			stream << "Command: " << ThrowableException<char>::getCommandLine() << std::endl;

			/*	If it has a exception backtrace.	*/
			if (stackEx) {
				stream << std::endl << stackEx->getBackTrace(StackColorPalette::StackColorDefault);
			}
		} else {
			/*	A normal std::exception.	*/
			stream << exception.what();
		}

		return stream.str();
	}

	/**
	 * @brief
	 *
	 * @tparam T
	 * @param ex
	 */
	template <class T> static void printStackMessage(const T &exception) noexcept {
		std::cerr << getStackMessage<T>(exception);
	}

	using CaptureException = ThrowableException<ExceptionChar>;

	class RuntimeException : public StackException<> {
	  public:
		RuntimeException() : StackException<>("RuntimeException") {}
		RuntimeException(RuntimeException &&other) = default;
		RuntimeException(const DefaultExcepCXXString &arg) : StackException(arg) {}
		template <typename... Args>
		RuntimeException(const DefaultExcepCXXString &format, Args &&...args) : StackException(format, args...) {}
	};

	class PermissionDeniedException : public StackException<> {
	  public:
		PermissionDeniedException() : StackException("PermissionDeniedException") {}

		PermissionDeniedException(const DefaultExcepCXXString &arg) : StackException(arg) {}
		template <typename... Args>
		PermissionDeniedException(const DefaultExcepCXXString &format, Args &&...args)
			: StackException(format, args...) {}
	};

	class DivideByZeroException : public StackException<> {};

	class IOException : public StackException<> {};

	class InvalidArgumentException : public StackException<> {
	  public:
		InvalidArgumentException() : StackException("Invalid Argument") {}
		InvalidArgumentException(InvalidArgumentException &&other) = default;
		InvalidArgumentException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		InvalidArgumentException(const DefaultExcepCXXString &format, Args &&...args)
			: StackException(format, args...) {}
	};

	class NotImplementedException : public StackException<> {};
	class NotSupportedException : public StackException<> {
	  public:
		NotSupportedException() : StackException("Not Supported") {}
		NotSupportedException(NotSupportedException &&other) = default;
		NotSupportedException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		NotSupportedException(const DefaultExcepCXXString &format, Args &&...args) : StackException(format, args...) {}
	};

	class IndexOutOfRangeException : public StackException<> {
	  public:
		IndexOutOfRangeException() : StackException("IndexOutOfRangeException") {}
		IndexOutOfRangeException(IndexOutOfRangeException &&other) = default;
		IndexOutOfRangeException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		IndexOutOfRangeException(const DefaultExcepCXXString &format, Args &&...args)
			: StackException(format, args...) {}
	};

	class InvalidPointerException : public StackException<> {
	  public:
		InvalidPointerException() : StackException("IndexOutOfRangeException") {}
		InvalidPointerException(InvalidPointerException &&other) = default;
		InvalidPointerException(const DefaultExcepCXXString &message) : StackException(message) {}
		template <typename... Args>
		InvalidPointerException(const DefaultExcepCXXString &format, Args &&...args)
			: StackException(format, args...) {}
	};

	// TODO determine if shall be renamed to ErrnoException
	class SystemException : public StackException<> {
	  public:
		SystemException() : SystemException(errno) {}
		SystemException(int errno_nr) : StackException(strerror(errno_nr)) {}
		SystemException(SystemException &&other) = default;
		SystemException(const ExceptionText &message) : StackException(message) {}
		template <typename... Args>
		SystemException(int errno_nr, const std::error_category &ecat, const DefaultExcepCXXString &format,
						Args &&...args)
			: StackException(format, args...) {
			this->errnoStrMsg = std::strerror(errno_nr);
			// Generate message based on the error values.
			this->generateMessage("", ecat);
		}

		const char *what() const noexcept override { return errorMessage.c_str(); }

	  protected:
		void generateMessage(const ExceptionText &text, const std::error_category &ecat) {
			errorMessage = fmt::format("{} {} {}", text, ecat.name(), errnoStrMsg);
		}
		String errorMessage;
		const char *errnoStrMsg;
	};

} // namespace cxxexcept

#endif