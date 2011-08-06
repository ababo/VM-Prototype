#include "utils.h"
#include "mbuilder.h"
#include "../common/sarithm.h"
#include "../common/exception.h"

namespace Ant {
  namespace VM {

    using namespace std;
    using namespace Ant::Common;

    VarTypeId ModuleBuilder::addVarType(size_t count, size_t bytes) {
      if(vtypes.size() >= MB_UINT_MAX(2))
        throw RangeException();
      if(count > MB_UINT_MAX(4))
        throw RangeException();
      if(bytes > MB_UINT_MAX(2))
        throw RangeException();
      if(count ?
         !safeToMultiply(count, bytes) :
         !safeToAdd(bytes, sizeof(size_t)))
        throw RangeException();

      VarType vtype;
      vtype.count = count;
      vtype.bytes = bytes;

      vtypes.push_back(vtype);
      return VarTypeId(vtypes.size() - 1);
    }

    RegId ModuleBuilder::addReg(VarTypeId vtype) {
      if(regs.size() >= MB_UINT_MAX(2))
        throw RangeException();
      if(vtype >= vtypes.size())
        throw NotFoundException();

      regs.push_back(vtype);
      return RegId(regs.size() + RESERVED_REGS_COUNT - 1);
    }

    ProcId ModuleBuilder::addProc(unsigned int flags, RegId io) {
      if(procs.size() >= MB_UINT_MAX(2))
        throw RangeException();
      if(flags >= PFLAG_FIRST_RESERVED)
        throw FlagsException();
      if(io < RESERVED_REGS_COUNT)
        throw ArgumentException();
      if(io >= regs.size() + RESERVED_REGS_COUNT)
        throw NotFoundException();

      Proc proc;
      proc.flags = flags;
      proc.io = io;

      procs.push_back(proc);
      instrs.push_back(0);
      return ProcId(procs.size() - 1);
    }

    size_t ModuleBuilder::addProcInstr(ProcId id, const Instr &instr) {
      if(id >= procs.size())
        throw NotFoundException();
      if(instrs[id] >= MB_UINT_MAX(4))
        throw RangeException();

      size_t size = instr.size();
      procs[id].code.resize(procs[id].code.size() + size);
      copy(instr.data(), instr.data() + size, back_inserter(procs[id].code));
      return ++instrs[id];
    }

    void ModuleBuilder::createModule(Module &module) {

    }

  }
}
