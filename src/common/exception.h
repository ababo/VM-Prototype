#ifndef __COMMON_EXCEPTION_INCLUDED__
#define __COMMON_EXCEPTION_INCLUDED__

#include <exception>

namespace Ant {
  namespace Common {

    class Exception : public std::exception {
    public:
      virtual const char *what() const throw() { return "generic error"; }
    };

    class EncodingException : public Exception {
    public:
      virtual const char *what() const throw() { return "encoding error"; }
    };

  }
}

#endif // __COMMON_EXCEPTION_INCLUDED__
