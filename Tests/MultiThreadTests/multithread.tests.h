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

#define TESTBUFFERSIZE (1024)

class MultiThreadTest : public QObject {
Q_OBJECT
public:
    MultiThreadTest(QObject *parent = nullptr){}

signals:
    void startWork();

private:
    QCircularBuffer *m_circularBuffer;
    QSignalSpy *m_readyReadSignalSpy;
    Producer *m_producer;
    Consumer *m_consumerA;
    Consumer *m_consumerB;
    QThread *m_producerThread;
    QThread *m_consumerThreadA;
    QThread *m_consumerThreadB;

private slots:
    void initTestCase() {
        m_circularBuffer = new QCircularBuffer();
        QCOMPARE(m_circularBuffer->isOpen(),false);
        m_circularBuffer->initialize(TESTBUFFERSIZE);
        QCOMPARE(m_circularBuffer->isOpen(),true);
        QCOMPARE(m_circularBuffer->isReadable(),true);
        QCOMPARE(m_circularBuffer->isWritable(),true);
        m_readyReadSignalSpy = new QSignalSpy(m_circularBuffer, SIGNAL(readyRead()));
        QCOMPARE(true,m_readyReadSignalSpy->isValid());
    }

    void initializeProducersConsumers(){
        m_producer = new Producer(TESTBUFFERSIZE);
        m_producer->registerCircularBuffer(m_circularBuffer);
        m_producerThread = new QThread();
        m_producer->moveToThread(m_producerThread);
        connect(m_producerThread,SIGNAL(started()),m_producer,SLOT(initialize()));
        connect(this,&MultiThreadTest::startWork,m_producer,&Producer::doWork);

        m_consumerA = new Consumer(1,TESTBUFFERSIZE);
        m_consumerA->registerCircularBuffer(m_circularBuffer);
        m_consumerThreadA = new QThread();
        m_consumerA->moveToThread(m_consumerThreadA);
        connect(m_consumerThreadA,SIGNAL(started()),m_consumerA,SLOT(initialize()));
        connect(this,&MultiThreadTest::startWork,m_consumerA,&Consumer::doWork);

        m_consumerB = new Consumer(2,TESTBUFFERSIZE);
        m_consumerB->registerCircularBuffer(m_circularBuffer);
        m_consumerThreadB = new QThread();
        m_consumerB->moveToThread(m_consumerThreadB);
        connect(m_consumerThreadB,SIGNAL(started()),m_consumerB,SLOT(initialize()));
        connect(this,&MultiThreadTest::startWork,m_consumerB,&Consumer::doWork);

        m_circularBuffer->registerConsumer(1);
        m_circularBuffer->registerConsumer(2);
        
        m_producerThread->start();
        m_consumerThreadA->start();
        m_consumerThreadB->start();

        qDebug() << m_circularBuffer->m_readTails;
    }


};

#endif // MULTITHREAD_TESTS_H
