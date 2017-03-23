#include "MainMemory.h"
#include "../Architecture.h"
#include <QDebug>
#include <QtGlobal>
#include "serialization.h"

MainMemory::MainMemory(double delay){
    this->delay = delay;
    // Build the stating array.
    _contents.resize(MEMORY_CHUNKS);
}

MainMemory::~MainMemory(){
}

QueryResult* MainMemory::read(unsigned int address, unsigned int length){
    QVector<Value> result;
    int startInd1;
    int startInd2;
    int endInd1;
    int endInd2;
    indexPair(address, &startInd1, &startInd2);
    indexPair(address+length-1, &endInd1, &endInd2);
    // Load in first.
    if(_contents.at(startInd1).size() == 0) {
        // This memory is yet to be written to.
        Value zero = {0};
        result = QVector<Value>(qMin((int)length, (int)MEMORY_CHUNK_SIZE), zero);
    }
    else {
        // This memory has been written to.
        result = _contents.at(startInd1).mid(startInd2, length);
    }
    startInd1++;
    // Load in bits in future vector.
    while(startInd1 <= endInd1) {
        if(_contents.at(startInd1).size() == 0) {
            // This memory is yet to be written to.
            Value zero = {0};
            result += QVector<Value>(qMin((int)(length-result.size()), (int)MEMORY_CHUNK_SIZE), zero);
        }
        else {
            // This memory has been written to.
            result += _contents.at(startInd1).mid(0, length-result.size());
        }
        startInd1++;
    }
    return new QueryResult(result, delay);
}

QueryResult* MainMemory::read(unsigned int address){
    return read(address, 1);
}

double MainMemory::write(QVector<Value> *value, unsigned int address){
    for(int i = 0; i < value->size(); i++) {
      int ind1;
      int ind2;
      indexPair(address+i, &ind1, &ind2);
      if(_contents.at(ind1).size() == 0) {
        // Ooops!  This memory chunk has not been instantiated yet!
        _contents[ind1].resize(MEMORY_CHUNK_SIZE);
      }
      _contents[ind1].replace(ind2, value->at(i));
    }
    return delay;
}

double MainMemory::write(Value input, unsigned int address){
    QVector<Value> *tinyQVector = new QVector<Value>(1, input);
    double result = write(tinyQVector, address);
    delete tinyQVector;
    return result;
}

QString *MainMemory::save(){
    QVector<int> result;
    for(int i = 0; i < _contents.size(); i++) {
        result.push_back(_contents.at(i).size());
        for(int j = 0; j < _contents.at(i).size(); j++) {
            result.push_back(_contents.at(i).at(j).i);
        }
    }
    return serialize(&result);
}

void MainMemory::restore(QString *state){
    QVector<int> *stateVector = deserialize(state);

    // Remove old contents.
    _contents = QVector<QVector<Value> >();
    // Load in new contents.
    size_t remainingElements = 0;
    for(int stateIndex = 0; stateIndex < stateVector->size(); stateIndex++) {
        int nextVal = stateVector->at(stateIndex);
        if(remainingElements == 0) {
            // Looks like it's time for a new vector!
            _contents.push_back(QVector<Value>());
            remainingElements = nextVal;
        }
        else {
            // Add to the last vector.
            Value v = {nextVal};
            _contents.last().push_back(v);
            remainingElements--;
        }
    }
}

void MainMemory::indexPair(unsigned int address, int *firstIndex, int *secondIndex){
    *firstIndex = address >> 16;
    *secondIndex = address & 0xffff;
}

