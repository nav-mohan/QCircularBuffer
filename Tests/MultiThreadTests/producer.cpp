#include "producer.h"

Producer::Producer(qint64 bufferSize) : m_writeBufferSize(bufferSize){}

Producer::~Producer(){
    if(m_writeBuffer != nullptr)
        delete m_writeBuffer;
}

void Producer::initialize() {
    m_writeBuffer = (char*)malloc(m_writeBufferSize);
    memset(m_writeBuffer,0,m_writeBufferSize);
}

void Producer::registerCircularBuffer(QCircularBuffer *cbuf) {
    m_circularBuffer = cbuf;
}

void Producer::burst() {
    qint64 bytesWritten = m_circularBuffer->write(const_cast<const char*>(m_writeBuffer),m_writeBufferSize);
}

void Producer::run() {
    qint64 bytesRead;
    while (true)
        bytesRead = m_circularBuffer->write(const_cast<const char*>(m_writeBuffer),m_writeBufferSize);
}