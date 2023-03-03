#include "qcircularbuffer.h"
#include <memory>

qint64 QCircularBuffer::readTail(char *data, qint64 maxSize, qint64 consumerID)
{
    if(!m_lock.tryLockForRead())
        return 0;

    qint64 ti = m_readTails.value(consumerID);
    qint64 vi = m_validData.value(consumerID);
    maxSize = qMin(vi,maxSize);
    
    if(m_bufferSize > ti+maxSize){
        // qDebug("READ %lld: NO LOOP",maxSize);
        memcpy(data,m_buffer+ti,maxSize);
        ti+=maxSize;
    }
    else{
        // qDebug("READ %lld: LOOP AROUND",maxSize);
        memcpy(data, m_buffer+ti, m_bufferSize-ti);
        #if defined(IS_TESTING)
            qDebug("EMITTING consumerLoop %lld",consumerID);
            emit consumerLoop(consumerID);
        #endif // IS_TESTING
        memcpy(data+m_bufferSize-ti,m_buffer,maxSize-(m_bufferSize-ti));
        ti = maxSize - (m_bufferSize-ti);

    }

    m_readTails.insert(consumerID,ti);
    m_validData.insert(consumerID,vi-maxSize);
    m_lock.unlock();
    return maxSize;
}



qint64 QCircularBuffer::writeData(const char *data, qint64 maxSize)
{
    if(!m_lock.tryLockForWrite())
        return 0;
    
    if(m_bufferSize > m_writeHead + maxSize) {
        // qDebug("WRITE %lld: NO LOOP",maxSize);
        memcpy(m_buffer+m_writeHead,data,maxSize);
        QMap<qint64, qint64>::const_iterator i = m_readTails.constBegin();
        while (i != m_readTails.constEnd()) {
            if(m_writeHead < i.value() && i.value() <= m_writeHead+maxSize) {
                m_readTails.insert(i.key(),m_writeHead);
                m_validData.insert(i.key(),maxSize);
                #if defined(IS_TESTING)
                    qDebug("EMITTING consumerReset %lld",i.key());
                    emit consumerReset(i.key());
                #endif // IS_TESTING
            }
            else {
                m_validData.insert(i.key(),m_validData.value(i.key())+maxSize);
            }
            ++i;
        }
        m_writeHead += maxSize;
        qDebug("WRITEHEAD MOVED TO %lld",m_writeHead);
        m_lock.unlock();
        emit QIODevice::readyRead();
        return maxSize;
    }

    else {
        if(m_bufferSize > maxSize) {
            // qDebug("WRITE %lld: SIMPLE LOOP",maxSize);
            memcpy(m_buffer+m_writeHead,data,m_bufferSize-m_writeHead);
            #if defined(IS_TESTING)
                qDebug("EMITTING producerLoop");
                emit producerLoop();
            #endif // IS_TESTING 
            memcpy(m_buffer,data+m_bufferSize-m_writeHead,maxSize-(m_bufferSize-m_writeHead));
            QMap<qint64, qint64>::const_iterator i = m_readTails.constBegin();
            while (i != m_readTails.constEnd()) {
                if(i.value() > m_writeHead || i.value() <= maxSize-(m_bufferSize-m_writeHead)) {
                    m_readTails.insert(i.key(),m_writeHead);
                    m_validData.insert(i.key(),maxSize);
                    #if defined(IS_TESTING)
                        qDebug("EMITTING consumerReset %lld",i.key());
                        emit consumerReset(i.key());
                    #endif // IS_TESTING
                }
                else {
                    m_validData.insert(i.key(),m_validData.value(i.key())+maxSize);
                }
                ++i;
            }
            m_writeHead = maxSize-(m_bufferSize-m_writeHead);
            qDebug("WRITEHEAD MOVED TO %lld",m_writeHead);
            m_lock.unlock();
            emit QIODevice::readyRead();
            return maxSize;
        }
        else {
            // qDebug("WRITE %lld: WRITE LAST m_bufferSize BYTES",maxSize);
            memcpy(m_buffer,data+maxSize-m_bufferSize,m_bufferSize);
            QMap<qint64, qint64>::const_iterator i = m_readTails.constBegin();
            while (i != m_readTails.constEnd()) {
                m_readTails.insert(i.key(),0);
                m_validData.insert(i.key(),m_bufferSize);
                #if defined(IS_TESTING)
                    qDebug("EMITTING consumerReset %lld",i.key());
                    emit consumerReset(i.key());
                #endif // IS_TESTING
                ++i;
            }
            m_writeHead = 0;
            #if defined(IS_TESTING)
                qDebug("EMITTING producerLoop");
                emit producerLoop();
            #endif // IS_TESTING

            // qDebug("WRITEHEAD MOVED TO %lld",m_writeHead);
            m_lock.unlock();
            emit QIODevice::readyRead();
            return m_bufferSize;
        }
    }
}





















// this actually does nothing
qint64 QCircularBuffer::readData(char *data, qint64 maxSize)
{
    return maxSize;
}




QCircularBuffer::~QCircularBuffer()
{
    qDebug("deleting QCircularBuffer");
    close();
    delete m_buffer;
}

void QCircularBuffer::initialize()
{
    m_writeHead = 0;
    m_buffer = (char*)malloc(m_bufferSize);
    open(QIODevice::ReadWrite);
}

void QCircularBuffer::registerConsumer(qint64 consumerID)
{
    m_readTails.insert(consumerID,(qint64)0);
    m_validData.insert(consumerID,m_writeHead);
}

void QCircularBuffer::removeConsumer(qint64 consumerID)
{
    int itemsRemoved = m_readTails.remove(consumerID); // number of items removed
    if(itemsRemoved != 1)
        qDebug("ERROR: %d CONSUMERS WITH consumerID = %lld WERE REMOVED",itemsRemoved,consumerID);
    itemsRemoved = m_validData.remove(consumerID); // number of items removed
    if(itemsRemoved != 1)
        qDebug("ERROR: %d CONSUMERS WITH consumerID = %lld WERE REMOVED",itemsRemoved,consumerID);
}

void QCircularBuffer::resetBuffer()
{
    memset(m_buffer,0,m_bufferSize);
    QMap<qint64, qint64>::const_iterator i = m_readTails.constBegin();
    while (i != m_readTails.constEnd()) 
    {
        m_readTails.insert(i.key(),0);
        m_validData.insert(i.key(),0);
        ++i;
    }   
    m_writeHead = 0;
}

