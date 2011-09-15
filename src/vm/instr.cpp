#include <sstream>

#include "../exception.h"
#include "instr.h"
#include "mbuilder.h"
#include "utils.h"

namespace Ant {
  namespace VM {

    using namespace std;
    using namespace Ant;

    void Instr::set(VMCode code) {
      size_t size = reinterpret_cast<const Instr*>(code)->size();
      
      if(!size)
        throw EncodingException();

      copy(code, code + size, &op);
    }

#define VIRTUAL_CASE(op, left, mod, call) \
    case OPCODE_##op: left static_cast<mod op##Instr&>(*this).call; break;

#define VIRTUAL_CALL(left, mod, call, def) \
    switch(op) { \
      VIRTUAL_CASE(INC, left, mod, call); \
      VIRTUAL_CASE(DEC, left, mod, call); \
      VIRTUAL_CASE(ADD, left, mod, call); \
      VIRTUAL_CASE(SUB, left, mod, call); \
      VIRTUAL_CASE(MUL, left, mod, call); \
      VIRTUAL_CASE(JNZ, left, mod, call); \
      VIRTUAL_CASE(JUG, left, mod, call); \
      VIRTUAL_CASE(CPI1, left, mod, call); \
      VIRTUAL_CASE(CPI2, left, mod, call); \
      VIRTUAL_CASE(CPI4, left, mod, call); \
      VIRTUAL_CASE(CPI8, left, mod, call); \
      VIRTUAL_CASE(ALS, left, mod, call); \
      VIRTUAL_CASE(ALSR, left, mod, call); \
      VIRTUAL_CASE(FRS, left, mod, call); \
      VIRTUAL_CASE(FRSN, left, mod, call); \
      VIRTUAL_CASE(CPB, left, mod, call); \
      VIRTUAL_CASE(LDE, left, mod, call); \
      VIRTUAL_CASE(LDB, left, mod, call); \
      VIRTUAL_CASE(LDR, left, mod, call); \
      VIRTUAL_CASE(STE, left, mod, call); \
      VIRTUAL_CASE(STB, left, mod, call); \
      VIRTUAL_CASE(RET, left, mod, call); \
      default: left def; \
    }

    size_t Instr::size() const {
      VIRTUAL_CALL(return, const, size(), 0);
    }

    void Instr::setParam(uint64_t p) {
      ostringstream out;
      writeMBUInt(p, out);
      string str = out.str();
      copy(str.begin(), str.end(), dat);
    }

    void Instr::set2Params(uint64_t p1, uint64_t p2) {
      ostringstream out;
      writeMBUInt(p1, out);
      writeMBUInt(p2, out);
      string str = out.str();
      copy(str.begin(), str.end(), dat);
    }

    void Instr::set2Params2(uint64_t p1, int64_t p2) {
      ostringstream out;
      writeMBUInt(p1, out);
      writeMBInt(p2, out);
      string str = out.str();
      copy(str.begin(), str.end(), dat);
    }

    void Instr::set3Params(uint64_t p1, uint64_t p2, uint64_t p3) {
      ostringstream out;
      writeMBUInt(p1, out);
      writeMBUInt(p2, out);
      writeMBUInt(p3, out);
      string str = out.str();
      copy(str.begin(), str.end(), dat);
    }

    void Instr::set3Params2(uint64_t p1, uint64_t p2, int64_t p3) {
      ostringstream out;
      writeMBUInt(p1, out);
      writeMBUInt(p2, out);
      writeMBInt(p3, out);
      string str = out.str();
      copy(str.begin(), str.end(), dat);
    }

#define INSTR_ISSTREAM(in) \
    const char *buf = reinterpret_cast<const char*>(dat); \
    istringstream in(string(buf, MAX_INSTR_SIZE - 1));

    size_t Instr::size(int paramCount) const {
      INSTR_ISSTREAM(in);
      size_t size = 1;
      uint64_t val;

      for(int i = 0; i < paramCount; i++)
        size += readMBUInt(in, val);

      return size;
    }

    uint64_t Instr::getParam(int index) const {
      INSTR_ISSTREAM(in);
      uint64_t val;

      for(int i = 0; i <= index; i++)
        readMBUInt(in, val);

      return val;
    }

    int64_t Instr::getParam2(int index) const {
      INSTR_ISSTREAM(in);
      int64_t val;

      for(int i = 0; i <= index; i++)
        readMBInt(in, val);

      return val;
    }

    void Instr::regSpec(const ModuleBuilder &mbuilder, RegId reg,
			VarSpec &vspec) {
      mbuilder.regById(reg, vspec);
    }

    void Instr::vrefSpec(const ModuleBuilder &mbuilder, RegId reg,
			 uit32_t vref, VarSpec &vspec) {
      mbuilder.regById(reg, vspec);
      
      VarType vtype;
      mbuilder.VarTypeById(vspec.vtype, vtype);

      if(vref >= vtype.vrefs.count())
	throw TypeException();

      return vspec = vtype.vrefs[vref];
    }

    size_t Instr::regEltCount(const ModuleBuilder &mbuilder, RegId reg,
			      uint32_t vref) {

    }

    void Instr::assertRegExists(const ModuleBuilder &mbuilder, RegId reg) {
      mbuilder.assertRegExists(reg);
    }

    void Instr::assertRegAllocated(const ModuleBuilder &mbuilder, ProcId proc,
                                   RegId reg) {
      mbuilder.assertRegExists(reg);
      mbuilder.assertRegAllocated(proc, reg);
    }

    void Instr::assertRegHasBytes(const ModuleBuilder &mbuilder, ProcId proc,
                                  RegId reg, uint32_t bytes) {
      assertRegAllocated(mbuilder, proc, reg);

      VarSpec vspec;
      mbuilder.regById(reg, vspec);
      VarType vtype;
      mbuilder.varTypeById(vspec.vtype, vtype);

      if(vtype.bytes < bytes)
        throw TypeException();
    }

    void Instr::assertSameVarType(const ModuleBuilder &mbuilder, ProcId proc,
				  VarTypeId vtype1, VarTypeId vtype2) {

    }

    void Instr::assertCompatibleEltCounts(size_t from, size_t to) {

    }

    void Instr::applyStackAlloc(ModuleBuilder &mbuilder, ProcId proc,
                                RegId reg, bool asRef) {
      mbuilder.assertRegExists(reg);
      mbuilder.applyStackAlloc(proc, reg, asRef);
    }

    void Instr::applyStackFree(ModuleBuilder &mbuilder, ProcId proc,
			       uint32 regs) {
      mbuilder.applyStackFree(proc, regs);
    }

    void Instr::applyInstrOffset(ModuleBuilder &mbuilder, ProcId proc,
                                 ptrdiff_t offset) {
      mbuilder.applyInstrOffset(proc, offset);
    }

    void Instr::applyDefault(ModuleBuilder &mbuilder, ProcId proc) {
      mbuilder.applyDefault(proc);
    }

    void Instr::assertConsistency(ModuleBuilder &mbuilder, ProcId proc) const {
      VIRTUAL_CALL(, const, assertConsistency(mbuilder, proc), );
    }

    bool Instr::breaks() const {
      VIRTUAL_CALL(return, const, breaks(), false);
    }

    bool Instr::jumps() const {
      VIRTUAL_CALL(return, const, jumps(), false);
    }

    size_t Instr::jumpIndex(size_t index) const {
      VIRTUAL_CALL(return, const, jumpIndex(index), 0);
    }

  }
}
