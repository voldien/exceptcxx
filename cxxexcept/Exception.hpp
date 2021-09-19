#ifndef _EXECPT_CXX_H_
#define _EXECPT_CXX_H_ 1
#include <fmt/format.h>
#include <stdexcept>

namespace cxxexcept {

	template <class T> class Exception : public std::exception {


		Exception(const char *what) {}
		Exception(const std::string &what) : Exception(what.c_str()) {}
		template <typename... Args>
		Exception(const std::string &format, Args &&... args) : Exception(fmt::format(format, args...)) {}

		Exception(const Exception &) = default;
		Exception &operator=(const Exception &) = default;
		Exception(Exception &&) = default;
		Exception &operator=(Exception &&) = default;

		const std::string &getBackTrace(void) const;

		const char *what(void) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT;

		//virtual const char *getName(void) const noexcept = 0;

	  protected:
		void generateStackTrace(void);
		Exception(void) = default;

	  private:
		std::string stackTrace;
		//std::string what;
	};

	
} // namespace cxxexcept


#endif