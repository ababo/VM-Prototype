#ifndef __STRING_INCLUDED__
#define __STRING_INCLUDED__

#include <ostream>
#include <string>

#include "char.h"

namespace Ant {

  class String {
    friend std::ostream &operator<<(std::ostream &out, const String &str);
    public:
      class Iterator : public std::iterator<std::bidirectional_iterator_tag,
                                            Char> {
      public:
        Iterator(const String &str, bool end)
          : str(str.str) { i = end ? str.size() : 0; }

        Char operator*() const;

      protected:
        const std::string &str;
        int i;
      };

    String() {}
    String(const char *str);

    operator const std::string&() const { return str; }
    bool operator==(const String &str) const { return str.str == this->str; }

    const char *c_str() const { return str.c_str(); }
    size_t size() const { return str.size(); }
    size_t length() const;

    Iterator begin() const { return Iterator(*this, false); }
    Iterator end() const { return Iterator(*this, true); }

    void push_back(Char chr);

  protected:
    std::string str;
  };

}

#endif // __STRING_INCLUDED__
