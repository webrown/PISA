#ifndef COMPUTER_H
#define COMPUTER_H

#include <QList>
#include "memory/Cache.h"
#include "memory/Register.h"
#include "memory/MainMemory.h"

class Computer{
    public:
        /*
         * If this variable is true, then computer will stop with break (I am not hundred percent sure about how to implement "break functionality"
         * If this variable is false, all break will be ignored
         */
        bool breakEnabled;
        
        Register* regs = NULL;
        MainMemory* mem = NULL;
        Cache* topCache = NULL;

        Computer();
        ~Computer();
        
        /*
         * Run this computer for nCycle 
         * If nCycle = -1, run indefintely
         */
        void step(int nCycle);
        
        /*
         * Pause the cycle of computer
         * If pause is called, during step, then remember the current step that the computer was it
         */
        void pause();
        
        /*
         * Kill the current program
         * This is different from removing the computer, it simply flushes out memory and pipeline
         */
        void stop();


        /* 
         * Initiate computer, after step, pause, stop will be called only after init is called
         *
         */
        void init();

};   
#endif