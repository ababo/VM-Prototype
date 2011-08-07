#include "../common/exception.h"
#include "../common/sarithm.h"
#include "mbuilder.h"
#include "runtime.h"
#include "utils.h"

namespace {

  using namespace std;
  using namespace Ant::Common;

  template <class T>
  inline void setFixedArray(const vector<T> &it,
                            FixedArray<T> &target,
                            vector<T> &container) {
    copy(it.begin(), it.end(), back_inserter(container));
    target.set(&container[container.size() - it.size()], it.size());
  }

}

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

      copy(instr.data(), instr.data() + instr.size(),
           back_inserter(procs[id].code));
      return instrs[id]++;
    }

    void ModuleBuilder::fillVarTypes(Runtime::ModuleData &moduleData) const {
      size_t size = 0;
      for(int i = 0; i < vtypes.size(); i++)
        size += vtypes[i].vrefs.size() + vtypes[i].prefs.size();
      moduleData.refs.reserve(size);

      for(int i = 0; i < vtypes.size(); i++) {
        Runtime::VarTypeData vtypeData;
        const VarType &vtype = vtypes[i];

        vtypeData.count = vtype.count;
        vtypeData.bytes = vtype.bytes;
        setFixedArray(vtype.vrefs, vtypeData.vrefs, moduleData.refs);
        setFixedArray(vtype.prefs, vtypeData.prefs, moduleData.refs);

        moduleData.vtypes.push_back(vtypeData);
      }
    }

    void ModuleBuilder::fillProcs(Runtime::ModuleData &moduleData) const {
      size_t size = 0;
      for(int i = 0; i < procs.size(); i++)
        size += procs[i].code.size();
      moduleData.code.reserve(size);

      for(int i = 0; i < procs.size(); i++) {
        Runtime::ProcData procData;
        const Proc &proc = procs[i];

        procData.flags = proc.flags;
        procData.io = proc.io;
        setFixedArray(proc.code, procData.code, moduleData.code);

        moduleData.procs.push_back(procData);
      }
    }

    void ModuleBuilder::createModule(Module &module) {
      Runtime::ModuleData moduleData;
      fillVarTypes(moduleData);
      moduleData.regs = regs;
      fillProcs(moduleData);

      UUID id = id.generate();
      Runtime::instance().insertModuleData(id, moduleData);
      module.id(id);

      vtypes.clear();
      regs.clear();
      procs.clear();
      instrs.clear();
    }

  }
}
