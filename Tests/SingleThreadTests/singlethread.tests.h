#if !defined(SINGLETHREAD_TESTS_H)
#define SINGLETHREAD_TESTS_H

#include <QTest>
#include <QDebug>
#include "qcircularbuffer.h"
#include <QObject>
#include <QSignalSpy>

#define TESTBUFFERSIZE (1024)

class SingleThreadTest : public QObject {
Q_OBJECT
public:
    SingleThreadTest(QObject *parent = nullptr){}

private:
    QCircularBuffer *m_circularBuffer;
    QSignalSpy *m_readyReadSignalSpy;

private slots:
    void initTestCase()
    {
        m_circularBuffer = new QCircularBuffer();
        QCOMPARE(m_circularBuffer->isOpen(),false);
        m_circularBuffer->initialize(TESTBUFFERSIZE);
        QCOMPARE(m_circularBuffer->isOpen(),true);
        QCOMPARE(m_circularBuffer->isReadable(),true);
        QCOMPARE(m_circularBuffer->isWritable(),true);
        m_readyReadSignalSpy = new QSignalSpy(m_circularBuffer, SIGNAL(readyRead()));
        QCOMPARE(true,m_readyReadSignalSpy->isValid());
        qDebug() << "isSequential " << m_circularBuffer->isSequential();
    }

/**
 * @brief verify writeHead loops back when m_writeHead==TESTBUFFERSIZE
 * write 512 bytes. verify m_writeHead == 512
 * write 512 bytes. verify m_writeHead == 0;
 */
    void verify_write_1()
    {
        qDebug() << "*******************************************";
        char *writeBuffer = (char*)malloc(512);
        
        qint64 bytesWritten = m_circularBuffer->write(const_cast<const char*>(writeBuffer), 512);
        QCOMPARE(bytesWritten,512);
        QCOMPARE(m_circularBuffer->m_writeHead,512);

        bytesWritten = m_circularBuffer->write(const_cast<const char*>(writeBuffer), 512);
        QCOMPARE(m_circularBuffer->m_writeHead,0);
        m_circularBuffer->resetBuffer();
        delete writeBuffer;
    }

/**
 * @brief when sizeof(writeBuffer) > TESTBUFFERSIZE verify only last TESTBUFFERSIZE bytes of writeBuffer gets transferred
 * verify writeHead == 0;
 */
    void verify_write_3()
    {
        qDebug() << "*******************************************";
        char *writeBuffer = (char*)malloc(1025);
        qint64 bytesWritten = m_circularBuffer->write(const_cast<const char*>(writeBuffer), 1025);
        QCOMPARE(m_circularBuffer->m_writeHead,0);
        m_circularBuffer->resetBuffer();
        delete writeBuffer;
    }
    
    // 512 + 1536 = 0; because 1536 > maxbuffersize os it just grabs the last 1024 bytes and sets hi = 0
    void verify_write_4()
    {
        qDebug() << "*******************************************";
        char *writeBuffer = (char*)malloc(1536);
        qint64 bytesWritten;
        bytesWritten = m_circularBuffer->write(const_cast<const char*>(writeBuffer), 512);
        QCOMPARE(m_circularBuffer->m_writeHead,512);
        bytesWritten = m_circularBuffer->write(const_cast<const char*>(writeBuffer), 1536);
        QCOMPARE(m_circularBuffer->m_writeHead,0);
        m_circularBuffer->resetBuffer();
        delete writeBuffer;
    }
    
/**
* @brief verify ti loops around
* write 1024, read 512. verify hi==0, ti==512. 
* write 256. read 512. verify hi == 256, ti == 0. 
*/
    void verify_read_1()
    {
        qDebug() << "*******************************************";
        qint64 consumerID = QDateTime::currentDateTime().toSecsSinceEpoch();
        m_circularBuffer->registerConsumer(consumerID);
        char *writeBuffer = (char*)malloc(1024);
        memset(writeBuffer,'a',1024);
        char *readBuffer = (char*)malloc(512);
        memset(readBuffer,0,512);
        qint64 bytesWritten = m_circularBuffer->write(const_cast<const char*>(writeBuffer),1024);
        qint64 bytesRead = m_circularBuffer->readTail(readBuffer, 512, consumerID);
        QCOMPARE(m_circularBuffer->m_writeHead,0);
        QCOMPARE(bytesWritten,1024);
        QCOMPARE(bytesRead,512);
        QCOMPARE(m_circularBuffer->m_readTails.value(consumerID),512);
        
        memset(writeBuffer,'b',256);
        memset(readBuffer,0,512);
        bytesWritten = m_circularBuffer->write(const_cast<const char*>(writeBuffer),256);
        bytesRead = m_circularBuffer->readTail(readBuffer, 512, consumerID);
        QCOMPARE(m_circularBuffer->m_writeHead,256);
        QCOMPARE(bytesWritten,256);
        QCOMPARE(bytesRead,512);
        QCOMPARE(m_circularBuffer->m_readTails.value(consumerID),0);

        m_circularBuffer->resetBuffer();
        m_circularBuffer->removeConsumer(consumerID);
        delete writeBuffer;
        delete readBuffer;
    }
/**
* @brief verify ti does not overtake hi
* write 512, read 1024 : verify hi == ti == 512; verify bytesWritten == bytesRead == 512;
* write 768, read 1024 : verify hi == ti == 256; verify bytesWritten == bytesRead == 768;
*/
    void verify_read_2()
    {
        qDebug() << "*******************************************";
        qint64 consumerID = QDateTime::currentDateTime().toSecsSinceEpoch();
        m_circularBuffer->registerConsumer(consumerID);
        char *writeBuffer = (char*)malloc(512);
        memset(writeBuffer,'a',512);
        char *readBuffer = (char*)malloc(1024);
        qint64 bytesWritten = m_circularBuffer->write(const_cast<const char*>(writeBuffer),512);
        qint64 bytesRead = m_circularBuffer->readTail(readBuffer,1024,consumerID);
        QCOMPARE(m_circularBuffer->m_writeHead,512);
        QCOMPARE(bytesWritten,512);
        QCOMPARE(bytesRead,512);
        QCOMPARE(m_circularBuffer->m_readTails.value(consumerID),512);
        
        m_circularBuffer->resetBuffer();
        m_circularBuffer->removeConsumer(consumerID);
        delete writeBuffer;
        delete readBuffer;
    }

};

#endif // SINGLETHREAD_TESTS_H
