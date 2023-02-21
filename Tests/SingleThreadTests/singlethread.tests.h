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
    }


    // 512 + 512 = 0
    void verify_write_1()
    {
        qDebug() << "*******************************************";
        char *writeBuffer = (char*)malloc(512);
        m_circularBuffer->write(const_cast<const char*>(writeBuffer), 512);
        QCOMPARE(m_circularBuffer->m_headIndex,512);
        m_circularBuffer->write(const_cast<const char*>(writeBuffer), 512);
        QCOMPARE(m_circularBuffer->m_headIndex,0);
        m_circularBuffer->resetBuffer();
        delete writeBuffer;
    }

    
    // 512 + 513 = 1
    void verify_write_2()
    {
        qDebug() << "*******************************************";
        char *writeBuffer = (char*)malloc(513);
        m_circularBuffer->write(const_cast<const char*>(writeBuffer), 512);
        QCOMPARE(m_circularBuffer->m_headIndex,512);
        m_circularBuffer->write(const_cast<const char*>(writeBuffer), 513);
        QCOMPARE(m_circularBuffer->m_headIndex,1);
        m_circularBuffer->resetBuffer();
        delete writeBuffer;

    }


    // 0 + 1025 = 0; because 1025 > maxbufferSize so it just grabs the last 1024 bytes and sets hi = 0;
    void verify_write_3()
    {
        qDebug() << "*******************************************";
        char *writeBuffer = (char*)malloc(1025);
        m_circularBuffer->write(const_cast<const char*>(writeBuffer), 1025);
        QCOMPARE(m_circularBuffer->m_headIndex,0);
        m_circularBuffer->resetBuffer();
        delete writeBuffer;

    }


};

#endif // SINGLETHREAD_TESTS_H
