#include "qcircularbuffer.h"
#include <memory>


qint64 QCircularBuffer::writeData(const char *data, qint64 maxSize)
{

    // if the lock couldn't be acquired right away, discard the data and move on.
    // If I discard the data, then a consumer might overtake producer. So I'll play this by ear
    if(!m_lock.tryLockForWrite())
        return maxSize;

    qDebug("Writing data %lld",maxSize);
    qint64 space_left = m_bufferSize-m_headIndex;
    
    // if bufferSize is large enough to accomodate data withouth circling back around
    if(maxSize <= space_left)
    {
        qDebug("CASE 1");
        memcpy(m_buffer+m_headIndex,data,maxSize);
        // loop through all tail-indices in m_tailMap and reset any ti if hi <= ti <= hi+maxSize
        QMap<qint64, qint64>::const_iterator i = m_tailMap.constBegin();
        while (i != m_tailMap.constEnd()) 
        {
            if(m_headIndex <= i.value() && i.value() <= m_headIndex+maxSize)
                m_tailMap.insert(i.key(),0);
            ++i;
        }
        m_headIndex += maxSize;
        if(m_headIndex == m_bufferSize)
            m_headIndex = 0;
        m_lock.unlock();
        return maxSize;
    }
    // bufferSize is not large enough to accomodate the data straightaway. Some part of the data needs to be circled around
    else
    {
        // maxSize could be larger than bufferSize requiring more than two loop arounds. 
        // in this case we copy just the last m_bufferSize bytes of data[maxSize]
        if(maxSize > m_bufferSize)
        {
            qDebug("CASE 2");
            memcpy(m_buffer,data+maxSize-m_bufferSize,m_bufferSize);
            // reset all tail-indices and set hi to 0;
            QMap<qint64, qint64>::const_iterator i = m_tailMap.constBegin();
            while (i != m_tailMap.constEnd()) 
            {
                m_tailMap.insert(i.key(),0);
                ++i;
            }        
            m_headIndex = 0;
            m_lock.unlock();
            return maxSize;
        }

        // space_left < maxSize < m_bufferSize. Requiring a loop around
        else
        {
            qDebug("CASE 3");
            memcpy(m_buffer+m_headIndex,data,space_left);
            memcpy(m_buffer,data+space_left,maxSize-space_left);
            // loop through all tail-indices and reset any ti if hi <= ti or ti <= maxSize-space_left
            QMap<qint64, qint64>::const_iterator i = m_tailMap.constBegin();
            while (i != m_tailMap.constEnd())
            {
                if(m_headIndex <= i.value() || i.value() <= space_left)
                    m_tailMap.insert(i.key(),0);
                ++i;
            }
            m_headIndex = maxSize-space_left;
            m_lock.unlock();
            return maxSize;
        }
    }
}




qint64 QCircularBuffer::readData(char *data, qint64 maxSize)
{
    if(!m_lock.tryLockForRead())
        return 0;

    qDebug("Reading data %lld",maxSize);
    m_lock.unlock();
    return maxSize;
}



QCircularBuffer::~QCircularBuffer()
{
    qDebug("deleting QCircularBuffer");
    delete m_buffer;
}

void QCircularBuffer::initialize(qint64 bufferSize)
{
    m_bufferSize = bufferSize;
    m_headIndex = 0;
    m_buffer = (char*)malloc(m_bufferSize);
    open(QIODevice::ReadWrite);
}

void QCircularBuffer::registerConsumer(qint64 timeStamp)
{
    m_tailMap.insert(timeStamp,(qint64)0);
}

int QCircularBuffer::removeConsumer(qint64 timeStamp)
{
    int n_itemsRemoved = m_tailMap.remove(timeStamp);
    return n_itemsRemoved;
}

void QCircularBuffer::resetBuffer()
{
    memset(m_buffer,0,m_bufferSize);
    QMap<qint64, qint64>::const_iterator i = m_tailMap.constBegin();
    while (i != m_tailMap.constEnd()) 
    {
        m_tailMap.insert(i.key(),0);
        ++i;
    }   
    m_headIndex = 0;
}

