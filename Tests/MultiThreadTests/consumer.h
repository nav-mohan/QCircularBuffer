#if !defined(CONSUMER)
#define CONSUMER

#include <QDebug>
#include <QObject>
#include "qcircularbuffer.h"

class Consumer : public QObject {
Q_OBJECT
public:
    Consumer(qint64 ,qint64 );
    ~Consumer();

public slots:
    void initialize();
    void registerCircularBuffer(QCircularBuffer *cbuf);
    void doWork();

private:
    QCircularBuffer *m_circularBuffer;
    char *m_readBuffer = nullptr;
    qint64 m_readBufferSize;
    qint64 m_consumerID;

};

#endif // CONSUMER
