#include "BitwiseGroup.h"
#define __AND__ &
#define __NAND__(X,Y)  ~((X) & (Y))
#define __OR__ |
#define __NOR__(X,Y)  ~((X) | (Y))
#define __XOR__  ^ 
#define __NOT__  ~

int toIntBG(float v){
    Value x;
    x.f =v;
    return x.i;
}

Value AndInstruction::scalarOperation(int a, int b, int *wait) {
  *wait = 1;
  Value v;
  v.i = a __AND__ b;
  return v;
}

Value AndInstruction::scalarOperation(float a, float b, int *wait) {
  *wait = 1;
  Value v;
  v.f = toIntBG(a) __AND__ toIntBG(b);
  return v;
}

Value NandInstruction::scalarOperation(int a, int b, int *wait) {
  *wait = 1;
  Value v;
  v.i = __NAND__(a,b);
  return v;
}

Value NandInstruction::scalarOperation(float a, float b, int *wait) {
  *wait = 1;
  Value v;
  v.f = __NAND__(toIntBG(a),toIntBG(b));
  return v;
}
Value OrInstruction::scalarOperation(int a, int b, int *wait) {
  *wait = 1;
  Value v;
  v.i = a __OR__ b;
  return v;
}

Value OrInstruction::scalarOperation(float a, float b, int *wait) {
  *wait = 1;
  Value v;
  v.f = toIntBG(a) __OR__ toIntBG(b);
  return v;
}
Value NorInstruction::scalarOperation(int a, int b, int *wait) {
  *wait = 1;
  Value v;
  v.i = __NOR__(a, b);
  return v;
}

Value NorInstruction::scalarOperation(float a, float b, int *wait) {
  *wait = 1;
  Value v;
  v.f = __NOR__(toIntBG(a), toIntBG(b));
  return v;
}

Value XorInstruction::scalarOperation(int a, int b, int *wait) {
  *wait = 1;
  Value v;
  v.i = a __XOR__ b;
  return v;
}

Value XorInstruction::scalarOperation(float a, float b, int *wait) {
  *wait = 1;
  Value v;
  v.f = toIntBG(a) __XOR__ toIntBG(b);
  return v;
}

Value NotInstruction::scalarOperation(int a, int b, int *wait) {
  *wait = 1;
  Value v; 
  v.i = __NOT__ a;
  return v;
}

Value NotInstruction::scalarOperation(float a, float b, int *wait) {
  *wait = 1;
  Value v;
  v.f = __NOT__ toIntBG(a);
  return v;
}

