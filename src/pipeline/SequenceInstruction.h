#ifndef SEQUENCE_INSTRUCTION_H
#define SEQUENCE_INSTRUCTION_H
#include "StageData.h"
#include "UnaryInstruction.h"
class SequenceInstruction : public UnaryInstruction {
  public:
    virtual void decode(StageData *, Register *);
    virtual void execute(StageData *);
    virtual void memory(StageData *, MemoryStructure *);
    virtual void writeBack(StageData *, Register *);
    virtual QVector<char> registerDependencies(StageData *sd);
    virtual QVector<char> registersToSet(StageData *sd);
    virtual bool decodeDump(StageData *, Register *r);
};
#endif
