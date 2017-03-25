#include "ArithmeticOperation.h"

Value ArithmeticOperation::scalarOperation(int a, int b){
  (void) a;
  Value v;
  v.i = b;
  return v;
}
Value ArithmeticOperation::scalarOperation(float a, float b){
  (void) a;
  Value v;
  v.f = b;
  return v;
}
Value ArithmeticOperation::scalarOperation(float a, int b){
  (void) a;
  Value v;
  v.i = b;
  return v;
}
Value ArithmeticOperation::scalarOperation(int a, float b){
  (void) a;
  Value v;
  v.f = b;
  return v;
}

Value ArithmeticOperation::scalarOperation(bool arg1IsFloat, bool arg2IsFloat, Value a, Value b){
  if(!arg1IsFloat && !arg2IsFloat) return scalarOperation(a.i, b.i);
  if(!arg1IsFloat &&  arg2IsFloat) return scalarOperation(a.i, b.f);
  if( arg1IsFloat && !arg2IsFloat) return scalarOperation(a.f, b.i);
                                   return scalarOperation(a.f, b.f);
}

void ArithmeticOperation::execute(Register *registers, bool arg1IsImmediate, unsigned int arg1, unsigned int arg2, bool flag, QBitArray flags){
  if(arg1IsImmediate) {
    // Immediate operations
    Value arg1Value;
    arg1Value.asUInt = arg1;
    if(Register::isVectorIndex(arg2)) {
      // immediate is being broadcast
      QVector<Value> result;
      for(int i = 0; i < registers->readVector(arg2).size(); i++) {
        if(flags[i]) result += scalarOperation(Register::isFloatIndex(arg2), Register::isFloatIndex(arg2), arg1Value, registers->readVector(arg2).at(i));
      }
      registers->write(result, arg2);
    }
    else {
      // immediate is being applied to scalar
      if(flag) registers->write(scalarOperation(Register::isFloatIndex(arg1), Register::isFloatIndex(arg2), arg1Value, registers->read(arg2)), arg2);
    }
  }
  else {
    // Non-immediate operations
    if(!Register::isVectorIndex(arg1) && !Register::isVectorIndex(arg2)) {
      //pure scalar operation
      if(flag) registers->write(scalarOperation(Register::isFloatIndex(arg1), Register::isFloatIndex(arg2), registers->read(arg1), registers->read(arg2)), arg2);
    }
    if(!Register::isVectorIndex(arg1) &&  Register::isVectorIndex(arg2)) {
      // broadcast arg1 across arg2
      QVector<Value> result;
      for(int i = 0; i < registers->readVector(arg2).size(); i++) {
        if(flags[i]) result += scalarOperation(Register::isFloatIndex(arg1), Register::isFloatIndex(arg2), registers->read(arg1), registers->readVector(arg2).at(i));
      }
      registers->write(result, arg2);
    }
    if( Register::isVectorIndex(arg1) && !Register::isVectorIndex(arg2)) {
      // Reduce arg1 onto arg2
      for(int i = 0; i < registers->readVector(arg2).size(); i++) {
        if(flags[i]) registers->write(scalarOperation(Register::isFloatIndex(arg1), Register::isFloatIndex(arg2), registers->readVector(arg1).at(i), registers->read(arg2)), arg2);
      }
    }
    if( Register::isVectorIndex(arg1) &&  Register::isVectorIndex(arg2)) {
      // dot arg1 with arg2
      QVector<Value> result;
      for(int i = 0; i < registers->readVector(arg2).size(); i++) {
        if(flags[i]) result += scalarOperation(Register::isFloatIndex(arg1), Register::isFloatIndex(arg2), registers->readVector(arg1).at(i), registers->readVector(arg2).at(i));
      }
      registers->write(result, arg2);
    }
  }
}