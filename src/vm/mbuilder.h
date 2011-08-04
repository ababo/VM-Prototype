#ifndef __VM_MBUILDER_INCLUDED__
#define __VM_MBUILDER_INCLUDED__

#include "uuid.h"
#include "module.h"
#include "runtime.h"
#include "instr.h"

namespace Ant {
  namespace VM {

    class ModuleBuilder {
    public:
      ModuleBuilder(Runtime &rt) : rt(rt) {}

      VarTypeId addVarType(int bytes, int count);
      int addVarTypeVRef(VarTypeId id, VarTypeId vref);
      int addVarTypePRef(VarTypeId id, VarTypeId pref);

      RegId addReg(VarTypeId vtype);

      ProcId addProc(int flags, RegId io);
      int addProcInstr(ProcId id, const Instr &instr);

      void resetModule();
      const UUID &createModule();

    protected:
      Runtime &rt;
    };

  }
}

#endif // __VM_MBUILDER_INCLUDED__
