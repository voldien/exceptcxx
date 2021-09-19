/**
    FragEngine, A Two layer Game Engine.
    Copyright (C) 2017  Valdemar Lindberg

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
#ifndef _FRAG_CORE_NOTSUPPORTEDEXCEPTION_H_
#define _FRAG_CORE_NOTSUPPORTEDEXCEPTION_H_ 1
#include "IException.h"

namespace fragcore {
	/**
	 *	Class responsible for showing
	 *	the method is not supported.
	 */
	class FVDECLSPEC NotSupportedException : public IException {
	public:

		NotSupportedException(void) : IException("Functionality not supported") {}

		NotSupportedException(const std::string &arg) : IException(arg) {}
		template <typename... Args>
		NotSupportedException(const std::string &format, Args &&... args) : IException(format, args...) {}
		virtual const char *getName(void) const noexcept override {
			return typeid(this).name();
		}
	};
}

#endif
