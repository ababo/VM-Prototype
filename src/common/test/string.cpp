#include "all.h"
#include "../string.h"
#include "../exception.h"
#include "../../test/all.h"

namespace {

  using namespace Ant::Common;
  using namespace Ant::Test;

  const String subj = "Ant::Common::String";

#define NON_ASCII "\xD0\x9D\xD0\xB0\x20\xD1\x80" \
  "\xD1\x83\xD1\x81\xD1\x81\xD0\xBA\xD0\xBE\xD0\xBC"

  bool testCtor() {
    bool passed = true;

    try { String s = NON_ASCII; }
    catch (...) { passed = false; }

    if(passed) {
      passed = false;
      
      try { String s = NON_ASCII "\xD0"; }
      catch (const EncodingException&) { passed = true; }
      catch (...) {} 
    }

    return printTestResult(subj, "ctor", passed);
  }

  bool testLength() {
    String buf = NON_ASCII;

    return printTestResult(subj, "length", buf.length() == 10);
  }
}

namespace Ant {
  namespace Common {
    namespace Test {

      bool testString() {
        return
          testCtor() &&
          testLength();
      }

    }
  }
}