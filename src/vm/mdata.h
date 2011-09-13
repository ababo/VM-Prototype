#ifndef __VM_MDATA_INCLUDED__
#define __VM_MDATA_INCLUDED__

#include "../retained.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "runtime.h"

namespace Ant {
  namespace VM {

    struct Runtime::ModuleData : Retained<ModuleData> {
      struct LLVMContext;

      ModuleData(const UUID &id);

      uint32_t varTypeCount() const;
      uint32_t procTypeCount() const;
      uint32_t regCount() const;
      uint32_t procCount() const;

      void varTypeById(VarTypeId id, VarType &vtype) const;
      void procTypeById(ProcTypeId id, ProcType &ptype) const;
      void regById(RegId id, VarSpec &reg) const;
      void procById(ProcId id, Proc &proc) const;

      bool isPacked() const;
      bool isDropped() const;

      void pack();
      void unpack();
      void drop();

      void callProc(ProcId proc, Variable &io);

      void take(ModuleData& moduleData);

      void assertNotDropped() const;
      void assertUnpacked() const;

      void prepareLLVMFPM();
      void createLLVMFuncs();
      void prepareLLVMContext(LLVMContext &context);
      void emitLLVMCode(LLVMContext &context);
      template<uint8_t OP, llvm::Instruction::BinaryOps, uint64_t>
        void emitLLVMCodeUO(LLVMContext &context, const UOInstr<OP> &instr);
      template<uint8_t OP, llvm::Instruction::BinaryOps>
        void emitLLVMCodeBO(LLVMContext &context, const BOInstr<OP> &instr);
      template<uint8_t OP, class VAL>
        void emitLLVMCodeIMM(LLVMContext &context,
                             const IMMInstr<OP, VAL> &instr);
      template<uint8_t OP, llvm::ICmpInst::Predicate, uint64_t>
        void emitLLVMCodeUJ(LLVMContext &context, const UJInstr<OP> &instr);
      void emitLLVMCodeAST(LLVMContext &context, const ASTInstr &instr);
      void emitLLVMCodeFST(LLVMContext &context, const FSTInstr &instr);
      void emitLLVMCodeCPB(LLVMContext &context, const CPBInstr &instr);
      void emitLLVMCodeRET(LLVMContext &context, const RETInstr &instr);
      const llvm::Type *getLLVMTypeById(VarTypeId id) const;

      const UUID &id;
      bool dropped;

      std::vector<VarTypeData> vtypes;
      std::vector<ProcType> ptypes;
      std::vector<VarSpec> vrefs;
      std::vector<ProcTypeId> prefs;
      std::vector<VarSpec> regs;
      std::vector<ProcData> procs;
      std::vector<VMCodeByte> code;

      llvm::Module *llvmModule;
      llvm::FunctionPassManager *llvmFPM;
      llvm::ExecutionEngine *llvmEE;
    };

  }
}

#endif // __VM_MDATA_INCLUDED__
