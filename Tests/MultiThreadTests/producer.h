#if !defined(PRODUCER)
#define PRODUCER

#include <QDebug>
#include <QObject>
#include "qcircularbuffer.h"

class Producer : public QObject {
Q_OBJECT
public:
    Producer(qint64);
    ~Producer();

public slots:
    void initialize();
    void registerCircularBuffer(QCircularBuffer *cbuf);
    void doWork();

private:
    QCircularBuffer *m_circularBuffer;
    char *m_writeBuffer = nullptr;
    qint64 m_writeBufferSize;

};

#endif // PRODUCER
