#ifndef __VM_MBUILDER_INCLUDED__
#define __VM_MBUILDER_INCLUDED__

#include <cstddef>
#include <vector>

#include "runtime.h"

namespace Ant {
  namespace VM {

    class Instr;
    class Module;

    class ModuleBuilder {
      friend class Instr;
    public:
      uint32_t varTypeCount() const { return vtypes.size(); }
      uint32_t procTypeCount() const { return ptypes.size(); }
      uint32_t regCount() const { return regs.size(); }
      uint32_t procCount() const { return procs.size(); }

      void varTypeById(VarTypeId id, VarType &vtype) const {
        vtype = vtypes[assertVarTypeExists(id)];
      }
      void procTypeById(ProcTypeId id, ProcType &ptype) const {
        ptype = ptypes[assertProcTypeExists(id)];
      }
      void regById(RegId id, VarSpec &reg) const {
        reg = regs[assertRegExists(id)];
      }
      void procById(ProcId id, Proc &proc) const {
        proc = procs[assertProcExists(id)];
      }

      VarTypeId addVarType(size_t bytes);
      void addVarTypeVRef(VarTypeId id, VarTypeId vtype, size_t count = 1);
      void addVarTypePRef(VarTypeId id, ProcTypeId ptype);
      ProcTypeId addProcType(uint32_t flags, RegId io);
      RegId addReg(VarTypeId vtype, size_t count = 1);
      ProcId addProc(uint32_t flags, ProcTypeId ptype);
      size_t addProcInstr(ProcId id, const Instr &instr);

      void resetModule();
      void createModule(Module &module);

    protected:
      struct ProcCon {
        typedef std::vector<size_t> Frame;
        std::vector<Frame> frames;
        std::vector<RegId> allocs;
        size_t instrCount;
      };

      VarTypeId assertVarTypeExists(VarTypeId id) const;
      VarTypeId assertProcTypeExists(VarTypeId id) const;
      RegId assertRegExists(RegId id) const;
      ProcId assertProcExists(ProcId id) const;
      RegId assertRegAllocated(ProcId proc, RegId reg) const;
      size_t assertCountInRange(VarTypeId vtype, size_t count);

      void fillVarTypes(Runtime::ModuleData &moduleData) const;
      void fillProcs(Runtime::ModuleData &moduleData) const;

      void applyStackAlloc(ProcId proc, RegId reg);
      void applyStackFree(ProcId proc);
      void applyInstrOffset(ProcId proc, int offset);
      void applyInstrIndex(ProcId proc, size_t index);
      void applyDefault(ProcId proc);

      void assertConsistency() const;

      std::vector<VarType> vtypes;
      std::vector<ProcType> ptypes;
      std::vector<VarSpec> regs;
      std::vector<Proc> procs;
      std::vector<ProcCon> procCons;
    };

  }
}

#endif // __VM_MBUILDER_INCLUDED__
