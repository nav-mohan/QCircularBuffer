#if !defined(CONSUMER_H)
#define CONSUMER_H

#include <QDebug>
#include <QObject>
#include "qcircularbuffer.h"

class Consumer : public QObject {
Q_OBJECT
public:
    Consumer(qint64 ,qint64 );
    ~Consumer();
    QCircularBuffer *m_circularBuffer;
    char *m_readBuffer = nullptr;
    qint64 m_readBufferSize;
    qint64 m_consumerID;

public slots:
    void initialize();
    void registerCircularBuffer(QCircularBuffer *cbuf);
    void burst();
    void run();

};

#endif // CONSUMER_H
