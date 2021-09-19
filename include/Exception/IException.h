/**
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
#ifndef _EXECPT_CXX_IEXCEPTION_H_
#define _EXECPT_CXX_IEXCEPTION_H_ 1
#include "../Def.h"
#include <fmt/format.h>
#include <stdexcept>

namespace execptcxx {

	/**
	 * @brief
	 *
	 */
	class FVDECLSPEC IException : public std::exception {
	  public:
		IException(void);
		IException(const char *what);
		IException(const std::string &what) : IException(what.c_str()) {}
		template <typename... Args>
		IException(const std::string &format, Args &&... args) : IException(fmt::format(format, args...)) {}

#if __cplusplus >= 201103L
		IException(const IException &) = default;
		IException &operator=(const IException &) = default;
		IException(IException &&) = default;
		IException &operator=(IException &&) = default;
#endif

		/**
		 * @brief Get the Back Trace object
		 *
		 * @return const std::string&
		 */
		const std::string &getBackTrace(void) const;

		const char *what(void) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT;

		virtual const char *getName(void) const noexcept = 0;

	  protected:
		void generateStackTrace(void);

	  private:
		std::string stackTrace;
		std::string mwhat;
	};
} // namespace execptcxx

#endif
