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
#include <cxxabi.h>
#include <errno.h>
#include <execinfo.h>
#include <fmt/format.h>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string.h>
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

		enum StackColorPalette {

		};

		const char *getExceptionName() const noexcept { return typeid(*this).name(); }

	  public:
		// template <class U> static const char *getExceptionName() noexcept { return typeid(U).name(); }



		/// proc/self/cmdline
		// GetCommandLineA
		/**
		 * @brief Get the Command Line object
		 *
		 * @return Text
		 */
		// TODO: implement getcommandline
		static std::string getCommandLine() noexcept {

			// std::string c(4096);
			// std::ifstream("/proc/self/cmdline").read(c, c.size());
			// return c;
		}

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

		virtual const Text &getBackTrace() const = 0;
		virtual Text getStackTree(unsigned int stackDepth) const noexcept = 0;

	  private:
		Text text;
	};

	// std::basic_string<Char>
	// std::basic_string<Char>
	// typename Char
	template <class T, class Text = ExceptionString> class StackException : public ExceptionBase<ExceptionString> {
		static_assert(std::is_object<Text>::value, "");

	  protected:
		StackException() {
			// TODO save the start address to start stacking from.
			generateStackTrace(32);
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

		virtual const Text &getBackTrace() const override {}

		Text getStackTree(unsigned int stackDepth) const noexcept { return Text(stackTrace.c_str()); }

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

	// TODO determine if shall be renamed
	class SystemException : public StackException<int> {
	  public:
		SystemException() : SystemException(errno) {}
		SystemException(int err) : StackException(strerror(errno)) {}
		SystemException(SystemException &&other) = default;
		SystemException(const ExceptionString &arg) : StackException(arg) {}
		template <typename... Args>
		SystemException(const ExceptionString &format, Args &&... args) : StackException(format, args...) {}
	};

	template <class U> static const char *getExceptionName() noexcept { return typeid(U).name(); }
	template <class U> static void printStackMessage(const U &ex) noexcept {
		std::cout << ex.what();
		//<< ex.getStackTree();
	}

	// using enable_if_t = typename std::enable_if<B, T>::type;
	using CaptureException = ThrowableException;

} // namespace cxxexcept

#endif