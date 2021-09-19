#ifndef _EXECPT_CXX_H_
#define _EXECPT_CXX_H_ 1

template <class T> class Exception : public std::exception {

	IException(void);
	IException(const char *what);
	IException(const std::string &what) : IException(what.c_str()) {}
	template <typename... Args>
	IException(const std::string &format, Args &&... args) : IException(fmt::format(format, args...)) {}

	const std::string &getBackTrace(void) const;

	const char *what(void) const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT;

	virtual const char *getName(void) const noexcept = 0;

  protected:
	void generateStackTrace(void);

  private:
	std::string stackTrace;
	std::string mwhat;
};

#endif