#include "../../common/string.h"
#include "../../test/all.h"
#include "../mbuilder.h"
#include "../module.h"

namespace Ant {
  namespace VM {
    namespace Test {

      void createFactorialModule(Module &module) {
        ModuleBuilder builder;

        VarTypeId wordType = builder.addVarType(1, 8);

        RegId io = builder.addReg(wordType);
        RegId pr = builder.addReg(wordType);

        ProcId proc = builder.addProc(PFLAG_EXTERNAL | PFLAG_FUNCTION, io);
        builder.addProcInstr(proc, ASTInstr(pr));
        builder.addProcInstr(proc, MOVM8Instr(1, pr));
        builder.addProcInstr(proc, UMULInstr(io, pr, pr));
        builder.addProcInstr(proc, DECInstr(io));
        builder.addProcInstr(proc, JNZInstr(-2));
        builder.addProcInstr(proc, MOVN8Instr(pr, io));
        builder.addProcInstr(proc, FSTInstr());
        builder.addProcInstr(proc, RETInstr());

        builder.createModule(module);
      }

    }
  }
}

namespace {

  using namespace Ant::VM;
  using namespace Ant::Test;
  using namespace Ant::Common;
  using namespace Ant::VM::Test;

  const String subj = "Ant::VM::ModuleBuilder";

  bool testConsistency() {
    bool passed = true;
    ModuleBuilder b;
    Module m;

    VarTypeId t = b.addVarType(1, 7);
    RegId r = b.addReg(t), n = r + 1;
    ProcId p = b.addProc(0, r);

    ASSERT_THROW({b.addProcInstr(p, FSTInstr());}, OperationException);
    ASSERT_THROW({b.addProcInstr(p, ASTInstr(n));}, NotFoundException);
    ASSERT_THROW({b.addProcInstr(p, MOVM8Instr(0, r));}, OperationException);
    ASSERT_THROW({b.addProcInstr(p, MOVM8Instr(0, n));}, NotFoundException);
    ASSERT_THROW({b.addProcInstr(p, MOVN8Instr(r, r));}, OperationException);
    ASSERT_THROW({b.addProcInstr(p, MOVN8Instr(n, n));}, NotFoundException);
    ASSERT_THROW({b.addProcInstr(p, UMULInstr(r, r, r));}, OperationException);
    ASSERT_THROW({b.addProcInstr(p, UMULInstr(n, n, n));}, NotFoundException);
    ASSERT_THROW({b.addProcInstr(p, DECInstr(n));}, OperationException);
    ASSERT_THROW({b.addProcInstr(p, DECInstr(n));}, NotFoundException);
    ASSERT_THROW({b.addProcInstr(p, JNZInstr(-1));}, RangeException);

    ASSERT_NOTHROW(b.addProcInstr(p, JNZInstr(1)));
    ASSERT_THROW(b.createModule(m), OperationException);
    ASSERT_NOTHROW(b.addProcInstr(p, RETInstr()));
    ASSERT_NOTHROW(b.createModule(m));
    IGNORE_THROW(m.drop());

    return printTestResult(subj, "consistency", passed);
  }

  bool testFactorialVTypes(const Module &module) {
    VarType vtype;
    bool passed;
    
    passed = module.varTypeCount() == 1;

    if(passed) {
      module.varTypeById(0, vtype);
      passed = vtype.count == 1 && vtype.bytes == 8;
      passed = passed && !vtype.vrefs.size() && !vtype.prefs.size();
    }

    ASSERT_THROW({module.varTypeById(1, vtype);}, NotFoundException);

    return passed;
  }

  bool testFactorialRegs(const Module &module) {
    bool passed;

    passed = module.regCount() == 2;
    passed = passed && module.regTypeById(RESERVED_REGS_COUNT) == 0;
    passed = passed && module.regTypeById(RESERVED_REGS_COUNT + 1) == 0;

    ASSERT_THROW({module.regTypeById(RESERVED_REGS_COUNT + 2);},
                 NotFoundException);

     return passed;
  }

#define NEXT_OPCODE(prefix) \
  if(passed) { \
    instr.set(&proc.code[i]); \
    i += instr.size(); \
    passed = passed && instr.opcode() == OPCODE_##prefix; \
}

#define NEXT_INSTR(prefix) \
  NEXT_OPCODE(prefix); \
  prefix##Instr &i##prefix = static_cast<prefix##Instr&>(instr);

  bool testFactorialInstrs(const Proc &proc) {
    bool passed = true;
    Instr instr;
    int i = 0;

    NEXT_INSTR(AST);
    passed = passed && iAST.reg() == RESERVED_REGS_COUNT + 1;
    NEXT_INSTR(MOVM8);
    passed = passed && iMOVM8.val() == 1; 
    passed = passed && iMOVM8.to() == RESERVED_REGS_COUNT + 1;
    NEXT_INSTR(UMUL);
    passed = passed && iUMUL.factor1() == RESERVED_REGS_COUNT;
    passed = passed && iUMUL.factor2() == RESERVED_REGS_COUNT + 1;
    passed = passed && iUMUL.product() == RESERVED_REGS_COUNT + 1;
    NEXT_INSTR(DEC);
    passed = passed && iDEC.it() == RESERVED_REGS_COUNT;
    NEXT_INSTR(JNZ);
    passed = passed && iJNZ.offset() == -2;
    NEXT_INSTR(MOVN8);
    passed = passed && iMOVN8.from() == RESERVED_REGS_COUNT + 1;
    passed = passed && iMOVN8.to() == RESERVED_REGS_COUNT;
    NEXT_OPCODE(FST);
    NEXT_OPCODE(RET);

    passed = passed && i == proc.code.size();

    return passed;
  }

  bool testFactorialProcs(const Module &module) {
    bool passed;
    Proc proc;

    passed = module.procCount() == 1; 

    if(passed) {
      module.procById(0, proc);
      passed = proc.flags == PFLAG_EXTERNAL | PFLAG_FUNCTION;
      passed = passed && proc.io == RESERVED_REGS_COUNT;
      passed = passed && testFactorialInstrs(proc);
    }

    ASSERT_THROW({module.procById(1, proc);}, NotFoundException);

    return passed;
  }

  bool testFactorial() {
    bool passed = true;
    Module module;

    try {
      createFactorialModule(module);

      passed = testFactorialVTypes(module);
      passed = passed && testFactorialRegs(module);
      passed = passed && testFactorialProcs(module);

      module.drop();
    }
    catch(...) { passed = false; }

    return printTestResult(subj, "factorial", passed);
  }

}

namespace Ant {
  namespace VM {
    namespace Test {

      bool testModuleBuilder() {
        bool passed;

        passed = testConsistency();
        passed = passed && testFactorial();

        return passed;
      }

    }
  }
}
