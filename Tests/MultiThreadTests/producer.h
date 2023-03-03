#if !defined(PRODUCER_H)
#define PRODUCER_H

#include <QDebug>
#include <QObject>
#include "qcircularbuffer.h"

class Producer : public QObject {
Q_OBJECT
public:
    Producer(qint64);
    ~Producer();
    QCircularBuffer *m_circularBuffer;
    char *m_writeBuffer = nullptr;
    qint64 m_writeBufferSize;

public slots:
    void initialize();
    void registerCircularBuffer(QCircularBuffer *cbuf);
    void burst();
    void run();

};

#endif // PRODUCER_H
