#include "WriteVectorElementOperation.h"
#include <QVector>
#include <QDebug>

WriteVectorElementOperation WriteVectorElementOperation::singleton;

void WriteVectorElementOperation::decodeOperation(Register *registers, Value arg1, Value arg2, Value arg3, int *wait) {
    *wait = 3+1+0+1;
    if(arg2.i < 0 || arg2.i >= VECTOR_SIZE) {
        qDebug() << "COM: WriteVectorElementOperation: NOOP index out of range.";
        return;
    }
    if(!Register::indexExists(arg3.i)) {
        qDebug() << "COM: WriteVectorElementOperation: NOOP " << arg3.i << " is not a valid index.";
        return;
    }
    if(!Register::isVectorIndex(arg3.i)) {
        qDebug() << "COM: WriteVectorElementOperation: NOOP vd should be a vector index.";
        return;
    }
    QVector<Value> vectorContents = registers->readVector(arg3.i);
    vectorContents[arg2.i] = arg1;
    registers->writeVector(vectorContents, arg3.i);
}
