#if !defined(MULTITHREAD_TESTS_H)
#define MULTITHREAD_TESTS_H
#include <QDebug>

#include <QTest>
#include "qcircularbuffer.h"
#include <QObject>
#include <QSignalSpy>
#include <QThread>
#include "producer.h"
#include "consumer.h"

#define TESTBUFFERSIZE (500)
#define PRODUCER_BUFFERSIZE (400)
#define CONSUMER_BUFFERSIZE (200)
#define LOOPS (2)

class MultiThreadTest : public QObject {
Q_OBJECT
public:
    MultiThreadTest(QObject *parent = nullptr){}

private:
    QCircularBuffer *m_circularBuffer;
    QThread *m_circularBufferThread;
    Producer *m_producer;
    Consumer *m_consumerA;
    Consumer *m_consumerB;
    QThread *m_producerThread;
    QThread *m_consumerThreadA;
    QThread *m_consumerThreadB;
    QSignalSpy *m_readyReadSignalSpy;
    QSignalSpy *m_producerLoopSignalSpy;
    QSignalSpy *m_consumerLoopSignalSpy;
    QSignalSpy *m_consumerResetSignalSpy;
    QSignalSpy *m_doneReadSignalSpy;


private slots:
    void initTestCase() {
        m_circularBuffer = new QCircularBuffer(TESTBUFFERSIZE);
        m_circularBufferThread = new QThread();
        m_circularBuffer->moveToThread(m_circularBufferThread);
        connect(m_circularBufferThread,&QThread::started,m_circularBuffer,&QCircularBuffer::initialize);
        QCOMPARE(m_circularBuffer->isOpen(),false);
        m_circularBufferThread->start();
        QTest::qWait(10);
        QCOMPARE(m_circularBuffer->isOpen(),true);
        QCOMPARE(m_circularBuffer->isReadable(),true);
        QCOMPARE(m_circularBuffer->isWritable(),true);
    }

    void initalize_signalSpies(){
        m_readyReadSignalSpy = new QSignalSpy(m_circularBuffer, &QIODevice::readyRead);
        m_producerLoopSignalSpy = new QSignalSpy(m_circularBuffer, &QCircularBuffer::producerLoop);
        m_consumerLoopSignalSpy = new QSignalSpy(m_circularBuffer, &QCircularBuffer::consumerLoop);
        m_consumerResetSignalSpy = new QSignalSpy(m_circularBuffer, &QCircularBuffer::consumerReset);
        m_doneReadSignalSpy = new QSignalSpy(m_circularBuffer, &QCircularBuffer::doneRead);

        m_readyReadSignalSpy->moveToThread(m_circularBufferThread);
        m_producerLoopSignalSpy->moveToThread(m_circularBufferThread);
        m_consumerLoopSignalSpy->moveToThread(m_circularBufferThread);
        m_consumerResetSignalSpy->moveToThread(m_circularBufferThread);
        m_doneReadSignalSpy->moveToThread(m_circularBufferThread);

        QCOMPARE(m_readyReadSignalSpy->isValid(),true);
        QCOMPARE(m_producerLoopSignalSpy->isValid(),true);
        QCOMPARE(m_consumerLoopSignalSpy->isValid(),true);
        QCOMPARE(m_consumerResetSignalSpy->isValid(),true);
        QCOMPARE(m_doneReadSignalSpy->isValid(),true);
        
        QCOMPARE(m_readyReadSignalSpy->count(),0);
        QCOMPARE(m_producerLoopSignalSpy->count(),0);
        QCOMPARE(m_consumerLoopSignalSpy->count(),0);
        QCOMPARE(m_consumerResetSignalSpy->count(),0);
        QCOMPARE(m_doneReadSignalSpy->count(),0);
    }

    void verify_producer_consumers_initialize_correctly(){
        m_producer = new Producer(PRODUCER_BUFFERSIZE);
        m_producer->registerCircularBuffer(m_circularBuffer);
        m_producerThread = new QThread();
        QVERIFY(m_producer->m_writeBuffer == nullptr);
        m_producer->moveToThread(m_producerThread);
        connect(m_producerThread,SIGNAL(started()),m_producer,SLOT(initialize()));

        m_consumerA = new Consumer(1,CONSUMER_BUFFERSIZE);
        m_consumerA->registerCircularBuffer(m_circularBuffer);
        m_consumerThreadA = new QThread();
        m_consumerA->moveToThread(m_consumerThreadA);
        connect(m_consumerThreadA,SIGNAL(started()),m_consumerA,SLOT(initialize()));

        m_consumerB = new Consumer(2,CONSUMER_BUFFERSIZE);
        // m_consumerB->registerCircularBuffer(m_circularBuffer);
        // m_consumerThreadB = new QThread();
        // m_consumerB->moveToThread(m_consumerThreadB);
        // connect(m_consumerThreadB,SIGNAL(started()),m_consumerB,SLOT(initialize()));

        m_circularBuffer->registerConsumer(1);
        // m_circularBuffer->registerConsumer(2);

        m_producerThread->start();
        m_consumerThreadA->start();
        // m_consumerThreadB->start();

        QTest::qWait(10); // put a small delay in the main thread while worker threads malloc

        // verify everything initialized correctly
        QCOMPARE(m_circularBuffer->m_readTails.value(1),0);
        QCOMPARE(m_circularBuffer->m_readTails.value(2),0);
        QVERIFY(m_producer->m_writeBuffer != nullptr);
        QVERIFY(m_consumerA->m_readBuffer != nullptr);
        // QVERIFY(m_consumerB->m_readBuffer != nullptr);
    }

    void verify_consumer_does_not_overtake_producer() {
        for (int i = 0; i < LOOPS; i++) {
            m_producer->burst();
            m_consumerA->burst();
            // m_consumerB->burst();
            QTest::qWait(100);//wait for the worker threads to finish their work otherwise we will prematurely lose scope when this function exits.
            qDebug() << m_circularBuffer->m_readTails << m_circularBuffer->m_writeHead;
            qDebug() << "m_readyReadSignalSpy " << m_readyReadSignalSpy->count();
            qDebug() << "m_producerLoopSignalSpy " << m_producerLoopSignalSpy->count();
            qDebug() << "m_consumerLoopSignalSpy " << m_consumerLoopSignalSpy->count();
            qDebug() << "m_consumerResetSignalSpy " << m_consumerResetSignalSpy->count();
            qDebug() << "m_doneReadSignalSpy " << m_doneReadSignalSpy->count();
            QCOMPARE(m_readyReadSignalSpy->count(),i+1);

        }
        // QVERIFY(m_circularBuffer->m_writeHead >= m_circularBuffer->m_readTails.value(1));
        // QVERIFY(m_circularBuffer->m_writeHead >= m_circularBuffer->m_readTails.value(2));
    }

};

#endif // MULTITHREAD_TESTS_H
