#include "DecodeStage.h"
#include "AddInstruction.h"
#include "BranchInstruction.h"
#include "CompareInstruction.h"
#include "LongAddInstruction.h"
#include "ReadVectorElementInstruction.h"
#include "WriteVectorElementInstruction.h"
#include "../memory/Flag.h"
#include "../Utility.h"
#define L_BIT  toB("1000000000000000000000")
#define L2_BIT toB("0000000100000000000000")



void DecodeStage::cycleUp(void){
    //Check delay
    delay--;
    if(delay > 0){
        return;
    } else{
        if(next->currData != NULL || dependencyFlag){
            //Structural hazard
            return;
        }
        else{
            next->currData = currData;
            currData = NULL;
        }
    }
}
using namespace Opcode;
void DecodeStage::cycleDown(void){
    if(currData == NULL){
        return;
    }

    uint vvv = currData->instruction;

    qDebug() << "COM: DecodeStage: Sees instruction " << intToBinary(currData->instruction) << ", PC of" << currData->instructionAddress;
    
    unsigned int flagCode = (vvv >> 28) & 15u;
    if(flagCode ==  0) currData->condFlag = Flag::EQ;
    if(flagCode ==  1) currData->condFlag = Flag::NE;
    if(flagCode ==  2) currData->condFlag = Flag::CS;
    if(flagCode ==  3) currData->condFlag = Flag::CC;
    if(flagCode ==  4) currData->condFlag = Flag::MI;
    if(flagCode ==  5) currData->condFlag = Flag::PL;
    if(flagCode ==  6) currData->condFlag = Flag::VS;
    if(flagCode ==  7) currData->condFlag = Flag::VC;
    if(flagCode ==  8) currData->condFlag = Flag::DZ;
    if(flagCode ==  9) currData->condFlag = Flag::NZ;
    if(flagCode == 10) currData->condFlag = Flag::LE;
    if(flagCode == 11) currData->condFlag = Flag::GE;
    if(flagCode == 12) currData->condFlag = Flag::LT;
    if(flagCode == 13) currData->condFlag = Flag::GT;
    if(flagCode == 14) currData->condFlag = Flag::AL;
    if(flagCode == 15) currData->condFlag = Flag::UN;

    currData->opcode = Opcode::opcodes[(vvv>>22 )& (63u)];


    if(currData->instructionFunctions == NULL) {
      // What instruction are we looking at?
      switch(currData->opcode){
          case Opcode::B: 
              currData->instructionFunctions = new BranchInstruction();
              break;
          case Opcode::BL:
              if(isDependent(vvv & 31)){
                  dependencyFlag= true;
                  return;
              }
              currData->dest = regs->read(vvv & 31);
              currData->destReg= 23;
              currData->regInUse = 1 << (vvv & 31);
              break;
          case Opcode::SEQ:
              if(isDependent(vvv & 31)){
                  dependencyFlag = true;
                  return;
              }
              //set type bit to vector;
              currData->info |= StageData::VECTOR;
              currData->info |= StageData::FLOAT;

              currData->destVec = regs->readVector(vvv & 31);
              currData->destReg= vvv & 31;
              currData->regInUse = 1 << (vvv & 31);
              break;
          case Opcode::WVE:
              currData->instructionFunctions = new WriteVectorElementInstruction();
              break;

          case Opcode::RVE:
              currData->instructionFunctions = new ReadVectorElementInstruction();
              break;

          case Opcode::CMP:
              currData->instructionFunctions = new CompareInstruction();
              break;

          case Opcode::LOD:
          case Opcode::STO:
          case Opcode::MVD:
          case Opcode::MVU:
          case Opcode::ARR:
          case Opcode::MOE:
          case Opcode::SOE:

          case Opcode::ADD:
          case Opcode::ADDS:
              currData->instructionFunctions = new AddInstruction();
              break;
          
          case Opcode::ADC:
              currData->instructionFunctions = new LongAddInstruction();
              break;
          
          case Opcode::ADCS:
          case Opcode::SUB:
          case Opcode::SUBS:
          case Opcode::SBC:
          case Opcode::SBCS:
          case Opcode::MUL:
          case Opcode::MULS:
          case Opcode::LMUL:
          case Opcode::LMULS:
          case Opcode::DIV:
          case Opcode::DIVS:
          case Opcode::LDIV:
          case Opcode::LDIVS:
          case Opcode::MOD:
          case Opcode::MODS:
          case Opcode::LSR:
          case Opcode::LSRS:
          case Opcode::LSL:
          case Opcode::LSLS:
          case Opcode::ASL:
          case Opcode::ASLS:
          case Opcode::AND:
          case Opcode::ANDS:
          case Opcode::NAND:
          case Opcode::NANDS:
          case Opcode::OR:
          case Opcode::ORS:
          case Opcode::NOR:
          case Opcode::NORS:
          case Opcode::XOR:
          case Opcode::XORS:
          case Opcode::NOT:
          case Opcode::NOTS:
              {
                  //check Float or not
                  if(isDependent(vvv&31)){
                      dependencyFlag= true;
                      return;
                  }
                  if(vvv & L_BIT == L_BIT){
                      currData->src = {(vvv & ((((1 << 16)-1))<<5))>>5};
                      currData->regInUse = 0;
                  }
                  else{
                      int _ = ((vvv & ((((1 << 5)-1))<<5))>>5);
                      if(isDependent(_)){
                          dependencyFlag= true;
                          return;
                      }
                      uint src = regs->read({_}).asUInt;
                      if(src < 24){
                          currData->src = regs->read(src);
                      }
                      else{
                          currData->info |= StageData::VECTOR;
                          currData->srcVec = regs->readVector(src);
                      }
                      currData->regInUse = 1 << src;
                  }
                  uint dest = regs->read(vvv & 31).asUInt;
                  if(dest < 24){
                      currData->dest = regs->read(dest);
                  }
                  else{
                      currData->info |= StageData::VECTOR;
                      currData->destVec = regs->readVector(dest);
                  }
                  currData->destReg = dest;
                  currData->regInUse |= 1 << dest;
                  break;
              }
          case Opcode::NOP:
          default:
              //Do nothing
              break;
      }
    }
    
    if(currData == NULL || currData->instructionFunctions == NULL) {
      // Something's gone wrong.  Don't die, just notify.
      qDebug() << "COM:  DecodeStage: Not sure what to do with opcode" << currData->opcode;
      return;
    }

    // Try to decode, just for the heck of it.  Might get dumped later.
    currData->instructionFunctions->decode(currData, regs);

    dependencyFlag = false;
    if(isDependent(currData)) {
        // Hey!  We can't decode this instruction yet!  Its dependencies are not resolved!
        dependencyFlag= true;
        return;
    }

    // OK!  Flags are settled!  We checked if anyone was messing with -1 in dependencies!
    currData->flagValue  = Flag::has(regs->readFlag(), currData->condFlag);
    currData->flagValues = Flag::has(regs->readFlags(), currData->condFlag);

    // If this instruction obeys the scalar flag or is malformed, we might be able to remove it now.
    if(currData->instructionFunctions->decodeDump(currData, regs) || currData->broken) {
        delete currData;
        currData = NULL;
    }
#if 1
qDebug() << "COM: DecodeStage: Newly decoded instruction:";
if(currData != NULL) {
  qDebug() << "COM: DecodeStage: src:"  << currData->src.i;
  qDebug() << "COM: DecodeStage: dest:" << currData->dest.i;
}
else {
  qDebug() << "COM: DecodeStage: NULLED out";
}
#endif
    delay = 1;
}

bool DecodeStage::isDependent(char regNum) const{
    for(Stage * curr = next ; curr != NULL ; curr = curr->next){
        if(curr->currData == NULL || curr->currData->instructionFunctions == NULL){
            continue;
        }
        QVector<char> willSet = curr->currData->instructionFunctions->registersToSet(curr->currData);
        for(int i = 0; i < willSet.size(); i++) {
            if(willSet.at(i) == regNum) {
                // Hey!  This stage is planning to modify this register!
                return true;
            }
        }
    }
    return false;
}

bool DecodeStage::isDependent(QVector<char> regNums) const{
    for(int i = 0; i < regNums.size(); i++) {
        // NOTE:  Got a SEG around here for a while because one of the functions was missing a return value for registerDependencies
        if(isDependent(regNums.at(i))){
            // Hey!  Some stage is planning to modify this register!
            return true;
        }
    }
    return false;
}

bool DecodeStage::isDependent(StageData *sd) const{
    if(sd == NULL || sd->instructionFunctions == NULL) return false;
    QVector<char> dependencies = sd->instructionFunctions->registerDependencies(sd);
    dependencies.push_back(-1); // -1 = condition flag.  EVERYONE depends on condition flags!
#if 0
qDebug() << "DEPENDENCIES:";
for(int i = 0; i < dependencies.size(); i++) qDebug() << QString::number(dependencies.at(i));
#endif
    return isDependent(dependencies);
}


