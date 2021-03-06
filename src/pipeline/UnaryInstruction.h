#ifndef UNARY_INSTRUCTION_H
#define UNARY_INSTRUCTION_H
#include "Instruction.h"
#include "../memory/Register.h"
#include "../memory/MemoryStructure.h"
class UnaryInstruction : public Instruction {
  public:
    virtual void decode(StageData *, Register *, int *wait);
};
#endif
