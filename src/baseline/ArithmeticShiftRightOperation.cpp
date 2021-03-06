#include "ArithmeticShiftRightOperation.h"
#include <QDebug>

ArithmeticShiftRightOperation ArithmeticShiftRightOperation ::singleton;

Value ArithmeticShiftRightOperation::scalarOperation(int a, int b, int *wait){
  *wait = 2+1+0+1;
  Value v;
  // Get rid of -.  - is where evil happens with ASR.
  bool mustSwap = false;
  if(b < 0) {
    mustSwap = true;
    b = ~b;
  }
  // Shift.
  b >>= a;
  // Swap back
  if(mustSwap) b = ~b;
  v.i = b;
  return v;
}

