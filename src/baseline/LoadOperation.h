#ifndef LOAD_OPERATION_H
#define LOAD_OPERATION_H
#include "../memory/MemoryStructure.h"
#include "../memory/Register.h"
#include "MemoryOperation.h"
class LoadOperation : public MemoryOperation{
    public:
        static LoadOperation singleton;
        void memoryOperation(Register *registers, MemoryStructure *memory, unsigned int address, unsigned int registerIndex, int *wait);
};
#endif
