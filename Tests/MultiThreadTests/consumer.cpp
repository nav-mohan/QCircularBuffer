#include "consumer.h"

Consumer::Consumer(qint64 consumerID, qint64 readBufferSize):m_consumerID(consumerID),m_readBufferSize(readBufferSize){}

Consumer::~Consumer(){
    if(m_readBuffer != nullptr)
        delete m_readBuffer;
}

void Consumer::initialize() {
    m_readBuffer = (char*)malloc(m_readBufferSize);
    memset(m_readBuffer,0,m_readBufferSize);
}

void Consumer::registerCircularBuffer(QCircularBuffer *cbuf) {
    m_circularBuffer = cbuf;
}

void Consumer::doWork() {
    qint64 bytesRead = m_circularBuffer->readTail(m_readBuffer,m_readBufferSize,m_consumerID);
}