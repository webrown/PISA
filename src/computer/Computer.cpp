#include "Computer.h"
#include <QDebug>

Computer::Computer(){
    qDebug() << "Creating computer";
    regs = new Register();
    mems = new MemoryStructure(MAIN_MEMORY_DELAY);
    exec = new Baseline(regs, mems);
}
Computer::~Computer(){
    qDebug() << "Removing Computer";
    delete regs;
    delete mems;
    qDebug() << "Computer is removed!";
}

void Computer::init(QVector<uint>* instructions){
    //demo!
    QVector<Value>* vec = new QVector<Value>();
    for(int i =0 ; i < instructions->size(); i++){
        uint instruction = instructions->at(i); 
        Value v = {instruction};
        vec->append(v);
    }
    mems->_mainMemory->write(vec, 0);

    //init 
    exec->init();
}

Status Computer::step(int nCycle){
    bool breakFlag = false;
    while(breakFlag == false && nCycle > 0){
        if(breakEnabled == true){
            uint pc = regs->getPC();
            if(breakMap.contains(pc) == true){
                BreakPoint::BreakPoint bp = breakMap[pc];
                if(bp == BreakPoint::BREAK){
                    breakFlag = true;
                    breakMap.remove(pc);
                }
                else if(bp == BreakPoint::BREAK_ALL){
                    breakFlag = true;
                }
                else if(bp == BreakPoint::SKIP || bp == BreakPoint::SKIP_ALL){
                    if(bp == BreakPoint::SKIP){
                        breakMap.remove(pc);
                    }
                    Value v = {regs->getPC()+1};
                    regs->w(Register::PC, v);
                    nCycle = nCycle < 0 ? -1 : nCycle -1;
                   continue;
                }
            }
        }
        Status status = exec->run();
        if(status != OKAY){
            return status;
        }
        nCycle = nCycle < 0 ? -1 : nCycle -1;
    }
}

void Computer::stop(){
    exec->stop();
}
void Computer::pause(){
    //Do nothing
}

void Computer::addBreakPoint(uint address, BreakPoint::BreakPoint bp){
    breakMap[address] = bp;
}

void Computer::removeBreakPoint(uint address){
    breakMap.remove(address);
}
