#ifndef __VM_TEST_ALL_INCLUDED__
#define __VM_TEST_ALL_INCLUDED__

#include "../module.h"

namespace Ant {
  namespace VM {
    namespace Test {

      void createFactorialModule(Module &module);

      bool testUtils();
      bool testModuleBuilder();
      bool testModule();

      bool testVM();
    }
  }
}

#endif // __VM_TEST_ALL_INCLUDED__
