#ifndef COPY_OPERATION_H
#define COPY_OPERATION_H
#include "ArithmeticOperation.h"
class CopyOperation : public ArithmeticOperation{
    public:
        static Value scalarOperation(int a, int b);
        static Value scalarOperation(float a, float b);
        static Value scalarOperation(float a, int b);
        static Value scalarOperation(int a, float b);
};
#endif