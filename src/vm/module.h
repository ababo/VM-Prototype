#ifndef __VM_MODULE_INCLUDED__
#define __VM_MODULE_INCLUDED__

namespace Ant {
  namespace VM {

    typedef unsigned int VarTypeId;
    typedef unsigned int FrameId;
    typedef unsigned int ProcId;

    enum ProcFlag {
      PFLAG_EXTERNAL = 0x1,
      PFLAG_FUNCTION = 0x2
    };

    class Module {

    };

  }
}

#endif // __VM_MODULE_INCLUDED__
